![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-32%20|%20win-64&color=blue)
![version](https://img.shields.io/badge/version-17%2B-3E8B93)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-opc)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-opc/total)

# 4d-plugin-opc

The OPC plugin reads and writes the parts, content types, and relationships inside Office Open XML packages — `.docx`, `.xlsx`, and `.pptx` files, all of which are ZIP containers built on the OPC (Open Packaging Conventions) format. It's a thin wrapper over [libopc](https://github.com/miyako/libopc): every command opens the target file as an OPC container, does one operation (read a part, list relations, write a part, add/remove a relation), and closes it. Results come back as 4D `Text`, `Longint`, `Blob`, and `Text array` values — there's no `Picture`/`Object` marshalling involved anywhere in this plugin.

**A note on command names below:** the plugin's own test file (`TEST.4dm`) confirms exactly one command's real 4D name — `OPC Get resource`. The other 17 names in this doc are derived from the plugin's internal C++ function names (`OPC_Get_document_type`, `OPC_RESOURCE_TYPE_LIST`, etc.) following that same naming pattern, but without the plugin's `manifest.json` I can't confirm the exact spelling/spacing/capitalization the plugin actually registers for each one. **Check the exact command name in 4D's method editor (autocomplete) or the plugin's `manifest.json` before relying on the spelling given here** — the parameter counts, types, order, and behavior in every section below, by contrast, are read directly from the plugin's source and are not guesses.

## Summary

| Command | Returns | Purpose |
|---|---|---|
| [`OPC Get document type`](#opc-get-document-type) | Text | Content type of the package's main document part |
| [`OPC Resource type list`](#opc-resource-type-list) | — | All registered content types in the package |
| [`OPC External target list`](#opc-external-target-list) | — | All external target URIs referenced anywhere in the package |
| [`OPC Extension list`](#opc-extension-list) | — | Default file-extension → content-type mappings |
| [`OPC Register resource type`](#opc-register-resource-type) | Text | Register a default content type for a file extension |
| [`OPC Relation type list`](#opc-relation-type-list) | — | All distinct relationship type URIs used in the package |
| [`OPC Get resource`](#opc-get-resource) | Longint | Read a part's raw bytes into a blob |
| [`OPC Resource list`](#opc-resource-list) | — | Recursively list internal parts reachable from a part |
| [`OPC Get resource size`](#opc-get-resource-size) | Longint | Byte size of a part |
| [`OPC Delete resource`](#opc-delete-resource) | Longint | Delete a part |
| [`OPC Set resource`](#opc-set-resource) | Longint | Create/overwrite a part with blob content |
| [`OPC External resource list`](#opc-external-resource-list) | — | Recursively list external targets reachable from a part |
| [`OPC Create relation`](#opc-create-relation) | Longint | Add an internal relationship between two parts |
| [`OPC Remove relation`](#opc-remove-relation) | Longint | Delete a relationship by id |
| [`OPC Create external relation`](#opc-create-external-relation) | Longint | Add a relationship to an external target |
| [`OPC Relation list`](#opc-relation-list) | — | Recursively list every relationship (internal + external) from a part |
| [`OPC Document type`](#opc-document-type) | Text | Look up the content-type string for a document kind constant |
| [`OPC Relation type`](#opc-relation-type) | Text | Look up the relationship-type URI for a relation kind constant |

**Platforms:** macOS (Intel & Apple Silicon) and Windows (32-bit & 64-bit). There is no platform-specific code in the plugin itself — every command goes through the same cross-platform `libopc` calls on both OSes, so there's no behavioral divergence to call out per-command below.

---

## Requirements & platform notes

- **Six commands return no usable function result: `OPC Resource type list`, `OPC External target list`, `OPC Extension list`, `OPC Relation type list`, `OPC Resource list`, `OPC External resource list`, and `OPC Relation list`.** Their C++ implementations never call the SDK's return-value function — call these as a plain statement, not as `$x:=...`. All the others (`OPC Get document type`, `OPC Get resource`, `OPC Get resource size`, `OPC Delete resource`, `OPC Set resource`, `OPC Register resource type`, `OPC Create relation`, `OPC Remove relation`, `OPC Create external relation`, `OPC Document type`, `OPC Relation type`) do return a value and are safe to assign.
- **Failure is usually a negative `Longint`, not a 4D error dialog.** These constants recur across the write/relation commands:

  | Constant | Value | Meaning |
  |---|---|---|
  | `OPC_ERROR_INVALID_PATH` | `-1` | The file couldn't be opened as an OPC container (bad path, not a valid ZIP/OPC package, or wrong read/write access) |
  | `OPC_ERROR_INVALID_RESOURCE_ID` | `-2` | The named part doesn't exist in the package |
  | `OPC_ERROR_INVALID_RELATION_ID` | `-3` | The relationship id/counter is invalid, or (for `OPC Remove relation`) not found |
  | `OPC_ERROR_INVALID_RELATION_TYPE` | `-4` | The relationship type string was empty |

  These aren't 4D constants exposed by the plugin — they're documented here as plain numbers because that's what the command hands back.
- **The read-only "list" commands (`OPC Resource type list`, `OPC External target list`, `OPC Extension list`, `OPC Relation type list`) have no error signal at all.** If the path is invalid, you simply get back an empty array — there's no way to distinguish "the package has none of these" from "the file couldn't be opened." If that distinction matters to your code, open the file with `OPC Get document type` first and check for a non-empty result.
- **A part path (used everywhere below as a "resource"/"part" parameter) is the same string libopc reports back from a listing command** — e.g. `/word/document.xml`. An empty part-path string is always treated as invalid up front (the plugin short-circuits before even asking libopc), so you can't address the package root by passing an empty string to `OPC Get resource`, `OPC Get resource size`, or `OPC Delete resource` — those three specifically require a real part path.
- **The traversal commands (`OPC Resource list`, `OPC External resource list`, `OPC Relation list`) treat an empty starting-part parameter differently — it means "start at the package root,"** not "invalid." Pass an empty string to list from the top of the package.
- **`OPC Set resource` always writes parts uncompressed** (`OPC_COMPRESSIONOPTION_NONE` is hard-coded) — there's no parameter to request compression.
- **`OPC Create external relation`'s target parameter currently has to name an existing part inside the same package, not an arbitrary external URI.** The command internally validates the destination with the same part lookup used for *internal* relations before adding it as an "external" relationship. In practice this means a genuinely external target (a web URL, a `mailto:` link) will fail with `OPC_ERROR_INVALID_RESOURCE_ID` (`-2`) rather than being accepted. This looks like it doesn't match the command's intent — worth confirming with whoever maintains the plugin before depending on it for real external targets; documented here as the command's actual current behavior, not its apparent design intent.
- Every path parameter accepts 4D's normal document-path syntax (the plugin funnels it through the SDK's own path-resolution helper before handing it to `libopc`) — pass the same kind of path you'd give `DOCUMENT TO BLOB` or `Structure file`.

---

## OPC Get document type

### Syntax
```4d
OPC Get document type ( path ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the `.docx`/`.xlsx`/`.pptx` file |
| Function result | Text | Content type of the package's main document part |

### Description
Opens the package, finds the root relationship of type `officeDocument`, follows it to the main part (`word/document.xml`, `xl/workbook.xml`, or `ppt/presentation.xml` depending on document kind), and returns that part's registered content type. If the file can't be opened, has no `officeDocument` relationship, or that relationship's target can't be resolved, the command returns an empty string — there's no distinct error code for this one.

### Example
```4d
$type:=OPC Get document type("C:\\Users\\me\\Documents\\report.docx")
If ($type#"")
    ALERT("Document type: "+$type)
Else
    ALERT("Could not read this file as an OPC package.")
End if
```

---

## OPC Resource type list

### Syntax
```4d
OPC Resource type list ( path ; contentTypes )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `contentTypes` | Text array | Filled with every distinct content type registered in `[Content_Types].xml` |

### Description
No function result — call it as a statement. On an invalid path the array simply comes back empty; there is no separate error signal.

### Example
```4d
ARRAY TEXT($types; 0)
OPC Resource type list("C:\\Users\\me\\Documents\\report.docx"; $types)
For ($i; 1; Size of array($types))
    ALERT($types{$i})
End for
```

---

## OPC External target list

### Syntax
```4d
OPC External target list ( path ; targets )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `targets` | Text array | Filled with every external target URI referenced anywhere in the package (hyperlinks, external images, etc.) |

### Description
No function result. This scans the whole package for external targets regardless of which part references them — it's not scoped to a single part (compare with [`OPC External resource list`](#opc-external-resource-list), which is scoped and also gives you the relationship type for each one).

### Example
```4d
ARRAY TEXT($targets; 0)
OPC External target list("C:\\Users\\me\\Documents\\report.docx"; $targets)
```

---

## OPC Extension list

### Syntax
```4d
OPC Extension list ( path ; extensions ; contentTypes )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `extensions` | Text array | Filled with each registered file extension (no leading dot, e.g. `xml`, `rels`, `png`) |
| `contentTypes` | Text array | Parallel array — the default content type registered for the extension at the same index |

### Description
No function result. This reads the `Default` entries of `[Content_Types].xml` — the extension-to-content-type map that applies to any part *not* individually overridden. Use [`OPC Register resource type`](#opc-register-resource-type) to add to this list.

### Example
```4d
ARRAY TEXT($ext; 0)
ARRAY TEXT($type; 0)
OPC Extension list("C:\\Users\\me\\Documents\\report.docx"; $ext; $type)
For ($i; 1; Size of array($ext))
    ALERT("."+$ext{$i}+" -> "+$type{$i})
End for
```

---

## OPC Register resource type

### Syntax
```4d
OPC Register resource type ( path ; extension ; contentType ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package (opened read-write) |
| `extension` | Text | File extension to register (no leading dot) |
| `contentType` | Text | Content type string to associate with the extension |
| Function result | Text | The content type actually registered |

### Description
Adds or confirms a `Default` content-type entry in `[Content_Types].xml`. If the container can't be opened, the function result is left unset (empty text). What happens if you register an extension that's already registered with a *different* content type isn't something the plugin's own source resolves — that's `libopc`'s call, so treat the returned string as the source of truth for what actually landed, rather than assuming it always echoes back the string you passed.

### Example
```4d
$registered:=OPC Register resource type("C:\\Users\\me\\Documents\\report.docx"; "svg"; "image/svg+xml")
```

---

## OPC Relation type list

### Syntax
```4d
OPC Relation type list ( path ; types )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `types` | Text array | Filled with every distinct relationship type URI used anywhere in the package |

### Description
No function result. Useful for a quick inventory of what kinds of relationships (styles, images, themes, hyperlinks, etc.) a document actually uses before deciding what to query with the relation-specific commands.

### Example
```4d
ARRAY TEXT($types; 0)
OPC Relation type list("C:\\Users\\me\\Documents\\report.docx"; $types)
```

---

## OPC Get resource

### Syntax
```4d
OPC Get resource ( path ; part ; content ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `part` | Text | Part path to read, e.g. `/word/document.xml`. Cannot be empty. |
| `content` | Blob | Filled with the part's raw bytes |
| Function result | Longint | `0` on success; `-1` if `path` is invalid; `-2` if `part` doesn't exist |

### Description
Streams the part's content into `content` in fixed-size chunks internally — there's no practical size limit imposed by the plugin itself beyond available memory. An empty `part` string is rejected immediately as invalid, before `libopc` is even asked, so you can't use this to fetch the package root.

### Example
From the plugin's own test method (`TEST.4dm`) — this specific call passes empty strings for both `path` and `part`, so it exercises the failure path (the file can't be opened) rather than a real read:
```4d
$a:=""
$b:=""
DOCUMENT TO BLOB:C525(Structure file:C489; $c)
$e:=OPC Get resource($a; $b; $c)
```
A realistic call with real arguments:
```4d
$path:="C:\\Users\\me\\Documents\\report.docx"
$blob:=New shared blob  // or any Blob variable
$result:=OPC Get resource($path; "/word/document.xml"; $blob)
If ($result=0)
    // $blob now holds the raw XML bytes of document.xml
Else
    ALERT("Read failed with code "+String($result))
End if
```

---

## OPC Resource list

### Syntax
```4d
OPC Resource list ( path ; part ; paths ; types )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `part` | Text | Part path to start from; pass an empty string to start at the package root |
| `paths` | Text array | Filled with the part path of every part reachable via *internal* relationships from `part`, recursively |
| `types` | Text array | Parallel array — the content type of each part at the same index |

### Description
No function result. This follows only internal relationships (a part pointing at another part inside the same package) and recurses depth-first — it does not report the relationships themselves (no id/name/type-of-relationship), just the parts found. Use [`OPC Relation list`](#opc-relation-list) if you need the relationship metadata too.

### Example
```4d
ARRAY TEXT($paths; 0)
ARRAY TEXT($types; 0)
OPC Resource list("C:\\Users\\me\\Documents\\report.docx"; ""; $paths; $types)  // whole package, from the root
```

---

## OPC Get resource size

### Syntax
```4d
OPC Get resource size ( path ; part ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `part` | Text | Part path to inspect. Cannot be empty. |
| Function result | Longint | Size in bytes; `-1` if `path` is invalid; `-2` if `part` doesn't exist |

### Description
The size is read from `libopc` as its own native integer type and narrowed to a 4D `Longint` (32-bit) by the plugin. For an ordinary document part this is never a concern, but if you're inspecting an unusually large embedded resource (multi-gigabyte video, for instance) be aware the reported size could wrap rather than reflect the true size.

### Example
```4d
$size:=OPC Get resource size("C:\\Users\\me\\Documents\\report.docx"; "/word/document.xml")
```

---

## OPC Delete resource

### Syntax
```4d
OPC Delete resource ( path ; part ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package (opened read-write) |
| `part` | Text | Part path to delete. Cannot be empty. |
| Function result | Longint | Result of the delete; `-1` if `path` is invalid; `-2` if `part` doesn't exist |

### Description
On a successful delete, the container is closed in a mode that also trims/reclaims space in the underlying ZIP; on any failure it's closed without trimming. Deleting a part does **not** remove relationships that pointed at it — clean those up separately with [`OPC Remove relation`](#opc-remove-relation) if you need a fully consistent package afterward.

### Example
```4d
$result:=OPC Delete resource("C:\\Users\\me\\Documents\\report.docx"; "/customXml/item1.xml")
```

---

## OPC Set resource

### Syntax
```4d
OPC Set resource ( path ; part ; contentType ; content ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package (opened read-write) |
| `part` | Text | Part path to create/overwrite, e.g. `/customXml/item1.xml` |
| `contentType` | Text | Content type to declare for this part. **Must be non-empty — this is what the command actually gates on, not `part`.** |
| `content` | Blob | Bytes to write |
| Function result | Longint | `0` on success (never explicitly set — reflects the variable's zero-initialized default); `-2` if `contentType` is empty or the part couldn't be created; `-1` if `path` is invalid |

### Description
Creates the part fresh via `libopc`'s part-creation call and writes `content` to it uncompressed (see the note in Requirements & platform notes — there's no way to request compression). If a part already exists at `part`, this plugin's own code doesn't special-case that — whether it overwrites, fails, or errors is `libopc`'s behavior at part-creation time, not something the plugin's source resolves either way.

### Example
```4d
$blob:=New shared blob
TEXT TO BLOB("<root/>"; $blob; UTF8 text without length)
$result:=OPC Set resource("C:\\Users\\me\\Documents\\report.docx"; "/customXml/item1.xml"; "application/xml"; $blob)
```

---

## OPC External resource list

### Syntax
```4d
OPC External resource list ( path ; part ; targets ; types )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `part` | Text | Part path to start from; empty string means the package root |
| `targets` | Text array | Filled with every external target URI reachable from `part`, following internal relationships recursively |
| `types` | Text array | Parallel array — the relationship type URI for each target at the same index |

### Description
No function result. Unlike [`OPC External target list`](#opc-external-target-list) (which scans the entire package unconditionally), this one is scoped to whatever's reachable from `part` and additionally reports each target's relationship type.

### Example
```4d
ARRAY TEXT($targets; 0)
ARRAY TEXT($types; 0)
OPC External resource list("C:\\Users\\me\\Documents\\report.docx"; ""; $targets; $types)
```

---

## OPC Create relation

### Syntax
```4d
OPC Create relation ( path ; sourcePart ; targetPart ; relationType ; id ; name ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package (opened read-write) |
| `sourcePart` | Text | Part the relationship originates from |
| `targetPart` | Text | Part the relationship points to (must already exist in the package) |
| `relationType` | Text | Relationship type URI. Cannot be empty. |
| `id` | Longint | Numeric suffix appended to `name` to form the relationship's id, e.g. `5` → `...5`. Must be `> 0`. |
| `name` | Text | Prefix for the relationship id. Optional — if empty, defaults to `rId` (so `id=5` with no `name` produces `rId5`) |
| Function result | Longint | The new relationship's status/id on success; `-4` if `relationType` is empty; `-3` if `id≤0` or the resulting id already exists; `-2` if `sourcePart` or `targetPart` doesn't exist; `-1` if `path` is invalid |

### Description
Both `sourcePart` and `targetPart` must resolve to existing parts already in the package — this command is for wiring up relationships between parts you've already created (e.g. with [`OPC Set resource`](#opc-set-resource)), not for external targets (use [`OPC Create external relation`](#opc-create-external-relation) for that, subject to the caveat noted for it above).

### Example
```4d
$result:=OPC Create relation("C:\\Users\\me\\Documents\\report.docx"; \
    "/word/document.xml"; "/customXml/item1.xml"; \
    "http://schemas.openxmlformats.org/officeDocument/2006/relationships/customXml"; \
    10; "")
    // creates relationship id "rId10"
```

---

## OPC Remove relation

### Syntax
```4d
OPC Remove relation ( path ; part ; id ; name ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package (opened read-write) |
| `part` | Text | Part the relationship originates from |
| `id` | Longint | Numeric suffix identifying which relationship to remove. Must be `> 0`. |
| `name` | Text | Prefix used to build the relationship id. Optional — defaults to `rId` if empty |
| Function result | Longint | Result of the delete on success; `-3` if `id≤0` or no relationship with that id/name exists; `-2` if `part` doesn't exist; `-1` if `path` is invalid |

### Description
Looks up the relationship named `name`+`id` (e.g. `rId10`) on `part` and deletes it if found. The container is always closed in the trim/reclaim mode whenever it opens successfully, whether or not a relationship was actually deleted.

### Example
```4d
$result:=OPC Remove relation("C:\\Users\\me\\Documents\\report.docx"; "/word/document.xml"; 10; "")
    // removes relationship id "rId10"
```

---

## OPC Create external relation

### Syntax
```4d
OPC Create external relation ( path ; sourcePart ; target ; relationType ; id ; name ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package (opened read-write) |
| `sourcePart` | Text | Part the relationship originates from |
| `target` | Text | Intended to be an external target (URI). **As currently implemented, this must resolve to an existing internal part path in the same package — see the caveat below.** |
| `relationType` | Text | Relationship type URI. Cannot be empty. |
| `id` | Longint | Numeric suffix appended to `name`. Must be `> 0`. |
| `name` | Text | Prefix for the relationship id. Optional — defaults to `rId` if empty |
| Function result | Longint | Status on success; `-4` if `relationType` is empty; `-3` if `id≤0` or the resulting id already exists; `-2` if `sourcePart` or `target` doesn't resolve; `-1` if `path` is invalid |

### Description
**Behavioral caveat, read before using this command:** despite the name and the "external" framing, the code validates `target` with the same internal part lookup used for `OPC Create relation`'s `targetPart` — it does not accept an arbitrary external URI. A real external target like `https://example.com` will fail with `-2` (invalid resource). This is documented here as the command's actual current behavior; it appears inconsistent with the command's evident purpose and is worth raising with whoever maintains the plugin before relying on it to link to genuinely external resources.

### Example
```4d
$result:=OPC Create external relation("C:\\Users\\me\\Documents\\report.docx"; \
    "/word/document.xml"; "/customXml/item1.xml"; \
    "http://schemas.openxmlformats.org/officeDocument/2006/relationships/customXml"; \
    11; "")
```

---

## OPC Relation list

### Syntax
```4d
OPC Relation list ( path ; part ; sources ; destinations ; names ; types )
```

| Parameter | Type | Description |
|---|---|---|
| `path` | Text | Path to the package |
| `part` | Text | Part path to start from; empty string means the package root |
| `sources` | Text array | Filled with the source part of each relationship found (empty string for a root-level relationship) |
| `destinations` | Text array | Parallel array — the target part path (internal) or target URI (external) |
| `names` | Text array | Parallel array — the relationship's id/name, including its numeric suffix (e.g. `rId5`) |
| `types` | Text array | Parallel array — the relationship type URI |

### Description
No function result. This is the most complete of the three traversal commands — it walks both internal and external relationships recursively from `part` and reports full relationship metadata (source, destination, id, type) for each one, rather than just the destination parts ([`OPC Resource list`](#opc-resource-list)) or just external targets ([`OPC External resource list`](#opc-external-resource-list)).

### Example
```4d
ARRAY TEXT($src; 0)
ARRAY TEXT($dst; 0)
ARRAY TEXT($name; 0)
ARRAY TEXT($type; 0)
OPC Relation list("C:\\Users\\me\\Documents\\report.docx"; ""; $src; $dst; $name; $type)
For ($i; 1; Size of array($dst))
    ALERT($name{$i}+": "+$src{$i}+" -> "+$dst{$i}+" ("+$type{$i}+")")
End for
```

---

## OPC Document type

### Syntax
```4d
OPC Document type ( kind ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `kind` | Longint | Document kind constant: `0` = docx, `1` = xlsx, `2` = pptx |
| Function result | Text | The content type string 4D/OPC uses to identify that document's main part |

### Description
A pure lookup — no file is opened. Any value other than `0`, `1`, or `2` returns an empty string. The plugin doesn't expose named 4D constants for these values (`OPC_DOCX`/`OPC_XLSX`/`OPC_PPTX` are C++-only); pass the literal number, or define your own project constants for readability.

### Example
```4d
C_LONGINT($kDocx; $kXlsx; $kPptx)
$kDocx:=0
$kXlsx:=1
$kPptx:=2
$type:=OPC Document type($kDocx)
```

---

## OPC Relation type

### Syntax
```4d
OPC Relation type ( kind ) -> Function result
```

| Parameter | Type | Description |
|---|---|---|
| `kind` | Longint | Relation kind constant, `0`–`6`: `0` image, `1` styles, `2` styles-with-effects, `3` settings, `4` web-settings, `5` font table, `6` theme |
| Function result | Text | The relationship type URI for that kind |

### Description
A pure lookup — no file is opened. Any value outside `0`–`6` returns an empty string. Useful for building the `relationType` argument to [`OPC Create relation`](#opc-create-relation) / [`OPC Create external relation`](#opc-create-external-relation) without hand-typing the schema URI.

### Example
```4d
C_LONGINT($kImage)
$kImage:=0
$relType:=OPC Relation type($kImage)
    // "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image"
$result:=OPC Create relation("C:\\Users\\me\\Documents\\report.docx"; "/word/document.xml"; "/word/media/image1.png"; $relType; 20; "")
```

---

## Error handling & troubleshooting

- **A negative `Longint` result, not a 4D error dialog, is how these commands report failure.** Check the function result against the four constants listed under Requirements & platform notes rather than wrapping calls in `ON ERR CALL`.
- **Six "list" commands return nothing you can assign** (`OPC Resource type list`, `OPC External target list`, `OPC Extension list`, `OPC Relation type list`, `OPC Resource list`, `OPC External resource list`, `OPC Relation list`) — call them as statements and read the array parameters afterward.
- **The four read-only list commands with no `part`/`type` scoping give you no way to tell "empty result" from "bad path."** If you need to know whether the file itself opened successfully, check it with `OPC Get document type` first.
- **`OPC Get resource`, `OPC Get resource size`, and `OPC Delete resource` treat an empty part path as automatically invalid** — you can't use them to address the package root; use the traversal commands (`OPC Resource list`, `OPC External resource list`, `OPC Relation list`) for that, which treat an empty starting part as "the root" instead.
- **`OPC Create external relation`'s target must currently be an existing internal part, not a real external URI** — see the caveat under that command's own section. Test with a real internal part path if you're getting an unexpected `-2`.
- **`OPC Set resource` always writes uncompressed.** If package size matters, be aware there's no way to opt into compression from 4D.
- **Deleting a part doesn't clean up relationships pointing at it.** Pair `OPC Delete resource` with `OPC Remove relation` if you need a self-consistent package afterward.
- **Optional trailing `name` parameters** (on `OPC Create relation`, `OPC Remove relation`, `OPC Create external relation`) default to `rId` when passed as an empty string — pass `""` explicitly rather than omitting the parameter if your 4D version requires all declared parameters to be supplied.

---

## Quick reference

```4d
// Inspect a package
$type:=OPC Get document type($path)
ARRAY TEXT($paths; 0) ; ARRAY TEXT($types; 0)
OPC Resource list($path; ""; $paths; $types)

// Read and write a part
$blob:=New shared blob
$err:=OPC Get resource($path; "/word/document.xml"; $blob)
$err:=OPC Set resource($path; "/customXml/item1.xml"; "application/xml"; $blob)

// Wire up a relationship between two existing parts
C_LONGINT($kImage) ; $kImage:=0
$relType:=OPC Relation type($kImage)
$err:=OPC Create relation($path; "/word/document.xml"; "/word/media/image1.png"; $relType; 20; "")

// Tear down
$err:=OPC Remove relation($path; "/word/document.xml"; 20; "")
$err:=OPC Delete resource($path; "/customXml/item1.xml")
```

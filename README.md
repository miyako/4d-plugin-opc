4d-plugin-opc
=============
A plugin to read and write DOCX, XLSX, PPTX files in 4D.

Implementation of [libopc](https://github.com/freuter/libopc) by Florian Reuter.

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
||<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Version

<img width="32" height="32" src="https://user-images.githubusercontent.com/1725068/73986501-15964580-4981-11ea-9ac1-73c5cee50aae.png"> <img src="https://user-images.githubusercontent.com/1725068/73987971-db2ea780-4984-11ea-8ada-e25fb9c3cf4e.png" width="32" height="32" />

Please consult the [wiki](https://github.com/miyako/4d-plugin-opc/wiki).

Commands
---

```c
// --- OPC Constants
OPC_Document_type
OPC_Relation_type

// --- OPC Resource
OPC_Get_resource
OPC_RESOURCE_LIST
OPC_Get_resource_size
OPC_Delete_resource
OPC_Set_resource
OPC_EXTERNAL_RESOURCE_LIST

// --- OPC Relation
OPC_Create_relation
OPC_Remove_relation
OPC_Create_external_relation
OPC_RELATION_LIST

// --- OPC Document
OPC_Get_document_type
OPC_RESOURCE_TYPE_LIST
OPC_EXTERNAL_TARGET_LIST
OPC_EXTENSION_LIST
OPC_Register_resource_type
OPC_RELATION_TYPE_LIST
```

Dependency
----------
Modified libopc, to accept Unicode file paths on Windows.  
Standard libiconv, libxml2, zlib included in package.

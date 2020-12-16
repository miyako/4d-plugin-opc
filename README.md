![platform](https://img.shields.io/static/v1?label=platform&message=osx-64%20|%20win-32%20|%20win-64&color=blue)
![version](https://img.shields.io/badge/version-17%2B-3E8B93)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-opc)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-opc/total)

4d-plugin-opc
=============
A plugin to read and write DOCX, XLSX, PPTX files in 4D.

Implementation of [libopc](https://github.com/freuter/libopc) by Florian Reuter.

Please consult the [wiki](https://github.com/miyako/4d-plugin-opc/wiki).

### Remarks

Modify ``file.c`` to accept Unicode file paths on Windows.  

Modify ``plib.h``

```c
#if _MSC_VER < 1900 
#define snprintf _snprintf
#endif
```

Supply ``iconv.h`` ``unistd.h`` for Windows.  

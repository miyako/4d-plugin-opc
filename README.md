![platform](https://img.shields.io/static/v1?label=platform&message=osx-64%20|%20win-32%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-opc)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-opc/total)

4d-plugin-opc
=============
A plugin to read and write DOCX, XLSX, PPTX files in 4D.

Implementation of [libopc](https://github.com/freuter/libopc) by Florian Reuter.

### Version

<img width="32" height="32" src="https://user-images.githubusercontent.com/1725068/73986501-15964580-4981-11ea-9ac1-73c5cee50aae.png"> <img src="https://user-images.githubusercontent.com/1725068/73987971-db2ea780-4984-11ea-8ada-e25fb9c3cf4e.png" width="32" height="32" />

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

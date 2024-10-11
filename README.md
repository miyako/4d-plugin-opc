![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-32%20|%20win-64&color=blue)
![version](https://img.shields.io/badge/version-17%2B-3E8B93)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-opc)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-opc/total)

4d-plugin-opc
=============
A plugin to read and write DOCX, XLSX, PPTX files in 4D.

Implementation of [libopc](https://github.com/miyako/libopc) by Florian Reuter.

Please consult the [wiki](https://github.com/miyako/4d-plugin-opc/wiki).

### Remarks for Apple Silicon

* apply `chmod +x configure` to all 3 configure files.
* change to `xmlValidateNCName` for error

```
../../../third_party/libxml2-2.7.7/debugXML.c:257:13: error: implicit declaration of function 'xmlValidateName' is invalid in C99 [-Werror,-Wimplicit-function-declaration]
```

* `#define LIBXML_THREAD_ENABLED 1` for error

```
../../../third_party/libxml2-2.7.7/threads.c:664:33: error: use of undeclared identifier 'xmlOnceInit'
    pthread_once(&once_control, xmlOnceInit);
```

* specify `--arch` and `--toolchain`

```
./configure --arch=arm64 --toolchain="clang" 
```

### Remarks

Modify ``file.c`` to accept Unicode file paths on Windows.  

Modify ``plib.h``

```c
#if _MSC_VER < 1900 
#define snprintf _snprintf
#endif
```

Supply ``iconv.h`` ``unistd.h`` for Windows.  

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

### Remarks

Modify ``file.c`` to accept Unicode file paths on Windows.  

Modify ``plib.h``

```c
#if _MSC_VER < 1900 
#define snprintf _snprintf
#endif
```

### __declspec(dllexport)和__declspec(dllimport)
在头文件的函数声明前加__declspec(dllexport)表示该函数是一个导出函数，可以被另外一个项目（dll或exe）调用。一个visualstudio项目里，只要有一个函数被声明成__declspec(dllexport)，那么在编译项目的时候，会同时生成lib文件，在其他项目调用这个函数并生成项目的时候，链接会用到lib文件。
__declspec(dllimport)表示该函数是一个导入函数，一个项目引用了另外一个项目里的导出函数的时候，需要在该函数的声明前加上__declspec(dllimport)的声明。
在一个项目引用另外一个项目的情况下，这两个项目会使用同一个头文件，一般都会这么定义：
#ifdef _API_EXPORT_
#define _API_EXPORT_ __declspec(dllexport)
#else
#define _API_EXPORT_ __declspec(dllimport)
#endif

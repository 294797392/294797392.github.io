#ifndef __EXCEPION_H__
#define __EXCEPION_H__

#include <setjmp.h>

/// 异常标记
typedef struct exception_s {
    int _exception_code;     // 异常类型，0表示无异常，异常类型号一般取小于0的数
	jmp_buf _stack_info; // 保存异常处理入口的堆栈信息
} exception_t;

/// 获取异常类型号
#define ExceptionCode(ex) ((ex)._exception_code)

/// 可能抛出异常的代码块
#define try(ex) if ( ((ex)._exception_code = setjmp((ex)._stack_info)) == 0 )

/// 捕获特定异常
#define catch(ex, code) else if ((ex)._exception_code == (code))

/// 捕获所有可能异常
#define catch_all(ex) else //if((ex)._exception_code < 0)

#define finally 

/// 抛出异常
#define throw(ex, code) longjmp((ex)._stack_info, code)



#endif //__EXCEPION_H__

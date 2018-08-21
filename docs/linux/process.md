## 进程信号

## 在父进程中获取子进程结束运行的方法
1.使用waitpid(pid, status, opt)函数，当子进程结束运行，waitpid会返回，否则会一直卡住
2.在父进程捕捉SIGCHLD信号。子进程终止时，内核会向父进程发送SIGCHLD信号。
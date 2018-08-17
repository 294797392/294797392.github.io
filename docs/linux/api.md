## dup2
`原型`
    int dup2(int oldfd, int newfd);
`描述`
    复制oldfd文件描述符，并且把oldfd的值设置成newfd的值，并关闭newfd。
    使oldfd指向newfd所指向的文件结构体。
    如果oldfd和newfd相同，返回newfd，不关闭newfd；如果oldfd和newfd不相同，把oldfd指向newfd所指向的文件结构体，关闭newfd。
`参数`
    oldfd：被复制的文件描述符
    newfd：被复制的文件描述符的新的文件描述符


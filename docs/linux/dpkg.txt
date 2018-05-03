*显示系统里安装的所有软件包		dpkg -l，dpkg --list
*显示所有与包相关的文件			dpkg -L [package]，dpkg --listfiles [package]

小技巧：
*可以结合grep命令根据关键字搜索相关的包：dpkg -l | grep [要搜索的包的关键字]

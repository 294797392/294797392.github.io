# GNU make

@[make|makefile|gcc]

在Linux开发中，通常使用make工具来编译项目，make工具使用makefile文件里定义的编译规则来编译项目，makefile会告诉make如何编译项目以及如何链接一个程序。当你写好一个makefile后，只需要在makefile所在目录打开终端，执行make命令，make就会根据makefile里制定的规则去编译项目了。本文介绍make工具的一些特性和用法。

[TOC]

## makefile
可以使用GNUmakefile，makefile，Makefile这三个里的其中一个来为makefile文件命名，也可以使用make工具的**-f name**或者**--file=name**选项来指定要使用的makefile。实际上，makefile只是自动的帮我们去运行写在makefile文件里的脚本，脚本里最核心的内容其实就是gcc等编译命令。个人感觉这和把gcc命令写在shell脚本里，然后用shell脚本去执行编译操作没多大区别。虽说可以直接把编译程序的命令和逻辑写在shell脚本里去执行，但是当项目很大，程序结构复杂，源码很多的时候，使用shell脚本编译就会显得非常复杂，但是make就不一样了，make有非常多的专为编译程序而设计的特性，可以帮助我们把很多复杂的编译操作化繁为简，make是专为编译而生的工具。

###  主要语法
下面是一个简单的makefile模板：
``` makefile
<target> : <prerequisites1> ... <prerequisitesN>
	<recipe1>
	   ..
	<recipeN>
``` 
- **target**：一个目标，一个makefile里会有非常多个目标，make工具会从第一个目标开始生成，如果目标依赖了另外一个目标，make会从被依赖的目标开始生成。

- **伪目标**：如果没有定义rerequisites（依赖目标），那么这个目标也被叫做伪目标，必须显示指定要执行的伪目标名称，才能执行伪目标。为了避免伪目标和makefile文件重名，可以使用“.PHONY <target>”来显示指定一个伪目标，这个指令会告诉makefile不管重名文件存不存在，都只执行伪目标命令。

- **rerequisites**：生成target所需要的文件或者是所依赖的其他target。一个target通常包含多个prerequisites，当然也可以不包含prerequisites，当target不包含prerequisites的时候，make会认为该target是最新的，在执行make命令的时候就不会去生成它。

- **recipe**：生成target所要执行的动作，一般情况下recipe就是gcc一系列的命令。注意，recipe需要以tab键开头，每行写一个recipe。

### 常用目标
all：编译所有目标
clean：删除所有被make创建的文件
install：安装编译好的程序，其实就是把编译出来的文件复制到指定的目录中去
print：列举改变过的源文件
tar：把源程序打成一个tar包
dist：把tar文件压缩成一个Z文件或者gz文件

### 变量
- 定义变量：`变量名=变量值`
- 引用变量：`$(变量名)`
- 追加变量值：`变量名 += 变量值`
- 目标变量：`<target>:变量名=变量值`，目标变量的作用域是target中以及连带target中，可以和全局变量同名，不会影响到全局变量的值。

### 函数

### 流程语句


## make命令

## GCC编译器


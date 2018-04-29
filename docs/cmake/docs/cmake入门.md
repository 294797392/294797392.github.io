## CMakeLists.txt文件编写入门
本文会用最简单的方式去编写和编译一个最简单的CMakeLists.txt文件，目的是想让读者快速入门cmake，并对CMakeLists.txt有一个直观的感受，从此对cmake不再望而却步。我的测试环境是ubuntu desktop，cmake的版本是3.5.1。cmake有非常多的功能，一时半会说不完那么多，在后面的章节里我会尽量讲解更多的cmake的特性和功能。以下内容都是作者通过阅读官方文档和实践总结出来的，可能会有错误，如果读者发现了错误，或者在阅读了本文之后，哪里不太理解，都请与我联系，谢谢，下面进入正文。

## 第一步：初始化CMakeLists.txt文件
首先，调用**cmake_minimum_required(VERSION 3.4)**命令设定要使用的cmake的版本，不同版本的cmake可能有不同的编译行为，不过这并不会影响到编译后的程序的正常运行。cmake的每个版本都有不同的policy，policy定义了cmake的编译行为，**cmake_minimum_required**命令内部调用了cmake_policy命令来设置cmake的policy，具体的每个版本的policy可以参考cmake官方文档。如果你系统里安装的版本比你指定的版本低，那么在编译的时候，cmake会停止编译并向你返回一个错误。


## 第二步：设置项目信息
调用**project(httpsvr VERSION 1.1 LANGUAGES C)**命令设置项目信息，调用了这个命令之后，cmake会在内部初始化许多环境变量的值，比如项目名称，语言，项目的版本号等，以便后续使用。以下列举一些被初始化的环境变量：
* **PROJECT_NAME**
* **PROJECT_SOURCE_DIR**
* **PROJECT_BINARY_DIR**
* **PROJECT_VERSION**
* **PROJECT_DESCRIPTION**

project命令格式：

**`project(<PROJECT-NAME> [VERSION <major>[.<minor>[.<patch>[.<tweak>]]]] [DESCRIPTION <project-description-string>] [LANGUAGES <language-name>...])`**

简单的说下这个格式的意思，尖括号里的表示是必须填写的选项，中括号里的表示的是可填也可以不填的选项。中括号里的大写的英文单词是固定的单词，使用中括号里的选项的时候，必须写上和格式里一样的单词，中括号里的LANGUAGES选项支持以下值：

**`C，CXX，CUDA，Fortran，ASM`**

如果你没有设置LANGUAGES选项，那么cmake会默认帮你把语言设置成C和CXX。如果你不想设置所使用的语言，也可以手动把LANGUAGES选项设置成NONE。

## 第三步：设置引用的头文件目录
使用**include_directories([PATH])**命令配置头文件目录,其中，PATH是要引用的头文件目录，这个路径是一个相对于CMakeLists.txt文件所在目录的相对目录，如果有多个头文件目录要引用，那么对每一个目录调用一次include_directories命令即可。注意，cmake知道系统头文件目录的位置，在CMakeLists.txt文件里不需要引用系统头文件的目录。

## 第四步：生成可执行文件或库文件
调用**add_library**命令生成库文件，调用**add_executable**命令生成可执行文件。  

下面分别简单介绍以下两个命令的含义

1. **`add_library(<name> [STATIC | SHARED | MODULE] [source1] [source2 ...])`**

其中，name参数是必须的，指定了要生成的库的名称，STATIC，SHARED，MODULE这些参数是可选的，指定了库类型，如果不指定库类型，那么cmake会把库类型默认设置为STATIC，生成的库的名称以lib+name开头，比如你定义了name是http，库类型是SHARED，那么生成之后的库名称就是libhttp.so，第三个参数是这个库使用的源文件，多个源文件用空格隔开，我测试了下，这个参数如果不写的话会报错。

add_executable命令格式：

2. **`add_executable(<name> [source1] [source2 ...])`**

add_executable命令参数的含义和add_library命令参数的含义一样，这里就不再赘述了，唯一的不同是add_executable命令可以直接生成可执行程序。注意，虽然这两个命令生成的是不同的东西，但是name不能一样，不然cmake会报错。  
我在cmake目录里写了一个CMakeLists.txt的测试文件，你可以这个文件做测试。
	
## 如何使用CMake编译项目
> **CMake使用CMakeLists.txt文件去进行编译，实际上，CMake只是根据CMakeLists.txt文件里的规则帮我们生成了Makefile，Makefile是make工具编译使用的文件，所以在使用CMake编译之后，还需要使用make命令再进行一次编译。**

最简单的，使用**cmake [PATH]**命令就可以编译了，其中，PATH是CMakeLists.txt文件所在的目录。  
下面举个最简单的使用cmake编译的例子：
1. 打开终端，进入CMakeLists.txt文件所在目录，执行cmake .（“.”代表当前目录）
2. 编译完成后，CMake会在当前目录生成很多文件，其中包括了一个名为Makefile的文件，如果你没有找到Makefile文件，那说明可能你并没有编译成功，有可能是CMakeLists.txt文件写的有问题。
3. 执行make命令，然后等待编译完成，编译完成后，会在当前目录生成编译出来的库文件或可执行文件。

## Definitions
Ps：数字参数
Pm：一个由多个数字参数组成的参数，使用分号隔开。用Ps参数表示参数个数
Pt：文本参数，由可打印的字符组成

## Single-character functions
BEL ：Bell（Ctrl+G），响铃
BS  ：Backspace（Ctrl+H），退格
CR  ：
ENQ ：
FF  ：
LF  ：
SO  ：
SP  ：
TAB ：
VT  ：
SI  ：

## Operation System Commands（OSC）：
OSC Ps;Pt ST ：
OSC Ps;Pt BEL：Ps=0 设置图标名称和窗口标题为Pt
               Ps=1 设置图标名称为Pt
               Ps=2 设置窗口标题为Pt
               Ps=3 设置主窗口的属性，Pt的格式有两种，一种是“prop=value”用来设置窗口属性；一种是“prop”用来重置（删除）窗口属性

## CSI Functions
CSI Ps K ：Erase in Line(EL)
           Ps=0 删除右边的一个字符
           Ps=1 删除左边的一个字符
           Ps=2 删除一行所有字符

## 参考
http://www.xfree86.org/4.7.0/ctlseqs.html
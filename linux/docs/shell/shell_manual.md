## if语句表达式大全
> 本节列举了在开发中常用的if语句表达式。

1. **字符串判断**  
<table>
<tr><th>表达式</th><th>含义</th></tr>
<tr><td>string1 = string2</td><td>string1和string2相等时返回true</td></tr>
<tr><td>string1 != string2</td><td>string1和string2不相等时返回true</td></tr>
<tr><td>-n string</td><td>当string的长度大于0时返回true</td></tr>
<tr><td>-z string</td><td>当string的长度等于0时返回true</td></tr>
<tr><td>string</td><td>当string不为空时返回true</td></tr>
</table>

2. **数字判断**  
<table>
<tr><th>表达式</th><th>含义</th></tr>
<tr><td>number1 -eq number2</td><td>number1和number2相等时返回true</td></tr>
<tr><td>number1 -ne number2</td><td>number1和number2不相等时返回true</td></tr>
<tr><td>number1 -gt number2</td><td>number1大于number2时返回true</td></tr>
<tr><td>number1 -ge number2</td><td>number1大于等于number2时返回true</td></tr>
<tr><td>number1 -lt number2</td><td>number1小于number2时返回true</td></tr>
<tr><td>number1 -le number2</td><td>number1小于等于number2时返回true</td></tr>
</table>

3. **文件和目录判断**  
<table>
<tr><th>表达式</th><th>含义</th></tr>
<tr><td>-r file</td><td>用户可以正常读取file时返回true</td></tr>
<tr><td>-w file</td><td>用户可以正常写入file时返回true</td></tr>
<tr><td>-x file</td><td>用户可以正常执行file时返回true</td></tr>
<tr><td>-f file</td><td>file存在则返回true，如果file是目录，则返回falsh</td></tr>
<tr><td>-d file</td><td>file为目录时返回true</td></tr>
<tr><td>-b file</td><td>file为块特殊文件时返回true</td></tr>
<tr><td>-c file</td><td>file为字符特殊文件时返回true</td></tr>
<tr><td>-s file</td><td>file大小大于0时返回true</td></tr>
<tr><td>-t file</td><td>当文件描述符（默认为1）指定的设备为终端时返回true</td></tr>
<tr><td>-e file</td><td>文件或目录存在则返回true</td></tr>
</table>

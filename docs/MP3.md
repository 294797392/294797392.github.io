## MP3文件格式


## ID3V2标签格式
+-----------------------------+
|      Header (10 bytes)      |
+-----------------------------+
|       Extended Header       |
| (variable length, OPTIONAL) |
+-----------------------------+
|   Frames (variable length)  |
+-----------------------------+
|           Padding           |
| (variable length, OPTIONAL) |
+-----------------------------+
| Footer (10 bytes, OPTIONAL) |
+-----------------------------+

ID3v2/file identifier      "ID3"
ID3v2 version              $04 00
ID3v2 flags                %abcd0000
ID3v2 size             4 * %0xxxxxxx


参考：http://id3.org/id3v2.4.0-structure
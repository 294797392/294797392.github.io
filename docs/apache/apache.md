### 安装apache2
ubuntu系统：sudo apt-get install apache2

### 配置文件路径
Ubuntu系统下：/etc/apache2/apache2.conf

### 新建虚拟主机（VirtualHost）
检查apache2.conf配置文件，需要包含如下两条配置：
IncludeOptional sites-enabled/*.conf
IncludeOptional sites-available/*.conf
在/etc/apache2/sites-available目录新建虚拟主机配置文件，在/etc/apache2/sites-enabled目录创建对虚拟主机配置文件的链接。
在/etc/apache2/apache2.conf文件里加入对新创建的虚拟主机的目录访问权限：
<Directory /var/mfgsvr/>
	Options Indexes FollowSymLinks
	AllowOverride None
	Require all granted
</Directory>
如果新建的虚拟主机使用了自定义的端口，需要修改/etc/apache2/ports.conf增加一个监听端口。
配置完之后，重启apache2服务。
注意，虚拟主机配置文件里的DucumentRoot目录和日志目录不能使用“~/”操作符，只能写相对路径或绝对路径。

### 日志目录
apache2的默认日志路径是/var/log/apache2，可以在/etc/apache2/apache2.conf文件对日志目录做修改。

## PKCS（Public-Key Cryptography Standards）标准
由美国RSA数据安全公司及其合作伙伴制定的一组公钥密码学标准，其中包括证书申请、证书更新、证书作废表发布、扩展证书内容以及数字签名、数字信封的格式等方面的一系列相关协议。
PKCS#8      私钥消息表示标准（Private-Key Information Syntax Standard）     Apache读取证书私钥的标准。
PKCS#10     证书申请标准（Certification Request Standard）	                参见RFC 2986。规范了向证书中心申请证书之CSR（certificate signing request）的格式。


## CULs
certificate revocation lists
证书吊销列表，列出被认为不能再使用的证书的序列号，openssl ca命令维护了一个CRLs


## pem格式和der格式
pem：扩展名pem用于ASCII(Base64)编码的各种X.509 v3 证书。文件开始由一行"—– BEGIN …“开始。
der：扩展名der用于二进制DER编码的证书。这些证书也可以用cer或者crt作为扩展名。比较合适的说法是“我有一个der格式编码的证书”，而不是“我有一个der证书”。
der类型的不用再次解码，直接就是二进制的数据可以直接使用，pem类型的数据要根据base64解码后，得到的数据需要进行增加或裁剪特殊字符-、\n、\r、begin信息、end信息等。


## openssl genrsa 生成非对称加密私钥（RSA private key）
openssl genrsa  [-out] 指定输出密钥的路径，如果不设置，则输出到stdout
                [-aes128|-aes192|-aes256|-aria128|-aria192|-aria256|-camellia128|-camellia192|-camellia256|-des|-des3|-idea] 指定加密私钥的对称加密算法。指定了该选项后，以后每次使用私钥都需要输入对称加密算法使用的密钥
                [numbits] 指定RSA加密算法使用的密钥的长度，bit为单位，默认为2048（256字节，RSA256）,这个选项必须是最后一个选项


## openssl req 生成PKCS#10标准定义的CSR（certificate signing request，证书签名请求）格式的文件
openssl req [-key] 指定私钥的所在目录。一般都是pem格式的文件和PKCS#8标准规定的私钥的格式。


## openssl ca 一个模拟CA机构的应用程序
openssl ca [-in ] 指定证书请求文件的目录（使用openssl req命令生成的文件）
           [-out] 指定生成的证书的输出目录，默认保存为pem格式。使用-spkac参数可以指定保存为der格式


## 生成客户端证书步骤：
1.生成私钥文件（openssl genrsa）
2.使用私钥生成证书请求文件（openssl req）
3.使用证书请求文件生成证书（openssl ca）

## 生成公钥私钥
ssh-keygen
#!/bin/sh
FILE="/bin/sh"

if [ -f "/home/zyf/code/294797392.github.io/linux/source/shell/if.sh" ];then
    echo "exit"
fi

exit

if [ ${FILE} = "/bin/sh" ]; then
    echo "/bin/sh"
fi

if [ ${FILE} != "/bin/dash" ]; then
    echo "FILE != /bin/dash"
fi

if [ -n ${FILE} ]; then
    echo "FILE长度大于0"
fi

if [ -z ${FILE} ]; then
    echo "FILE长度等于0"
fi

if [ ${FILE} ]; then
    echo "FILE非空"
fi
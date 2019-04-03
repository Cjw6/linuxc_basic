#使用变量
A=123
echo "$A"
echo  "first shell $A"
#使用系统变量
echo "uid"$UID
#使用命令
echo $PWD
echo "###############################"
# $0 表示脚本本身的名称
echo $0
# $1 是参数，命令行参数的第一个
echo $1
echo $2
#\$ 是转义字符 查看* # ? 是什么意思
#$?上个命令是否正确    
#$*所有命令行参数 $#命令行参数个数
echo "\$? is $?"
echo "\$* is $*"
echo "\$# is $#"
#打印颜色横线
echo  "\033[32m--------------------------------------\033[m"


echo "install linux"
echo "1:install ubuntu"
echo "2:install linux-mint"
echo "3:install debian"
echo "---------------------------"


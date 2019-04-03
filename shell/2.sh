#shell 的if 使用


#比较数字大小
num1=120
num2=200
if (($num1>$num2));then
    echo "max:num1"
else 
    echo "max:mum2"
fi

if (($num1<$num2));then
    echo "max:num2"
fi

#


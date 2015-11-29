#!/bin/sh


check1()
{
dat=$(cat $1 | grep $2)
if [ "$dat" = "" ]; then
echo "$1: no $2"
exit
fi
}

check2()
{
i=$1
nm=$(cat $i | grep "// File:" | gawk '{print $3}')
filename="${i##*/}"
if [ "$filename" = "$nm" ]; then
echo "$nm - ok"
else
echo "Error: bad file tag $1 [$filename] [$nm]"
exit
fi
}

checkdir()
{
for i in $1/*.h $1/*.cpp 
do

check1 $i "Author"
check1 $i "University"
check1 $i "Copyright"
check1 $i "CryptoLeq"

check2 $i

done
}

checkdir mmcalc
checkdir ceal
checkdir processor
checkdir cpppir

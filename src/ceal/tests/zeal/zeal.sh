#!/bin/sh

OS=$OS
if [ "$OS" = "Windows_NT" ]; then
exe=.exe
else
exe=
fi

bdir=../..
wdir=`pwd`
cas=$1

run()
{
name=ceal_$1_c$2_m$3$exe
echo runnung $name
cmd="./$name $cas"
echo $cmd
res1=`(time -p $cmd) 2>&1`
res2=`echo $res1 | grep real | gawk '{ print $8, $9, $10, $11, $12 }'` # 204
#res2=`echo $res1 | grep real | gawk '{ print $5 }'` # 1011
echo $name $res2 >> result
echo $res1
}

build()
{
name=ceal_$1_c$2_m$3$exe

cd $bdir
echo building $1 $2 $3
make clean
make GMP=1 CELLDEF=$2 MEMDEF=$3 PLAT=$1 -j 12
make GMP=1 CELLDEF=$2 MEMDEF=$3 PLAT=$1
cp _bin_$1/ceal$exe $wdir/$name
make clean
cd $wdir
echo make CELLDEF=$2 MEMDEF=$3 PLAT=$1
}

myecho()
{
echo "myecho $1 $2 $3"
}

list()
{
name=ceal_$1_c$2_m$3$exe
ls $name
}

level3()
{
$4 $1 $2 $3
}

level2()
{
level3 $1 $2 3 $3
level3 $1 $2 2 $3
level3 $1 $2 1 $3
}

level1()
{
level2 $1 4 $2
level2 $1 3 $2
level2 $1 2 $2
level2 $1 1 $2
}

level0()
{
#level1 msc $1
level1 unx $1
}

:>result
level0 build
level0 list
#level0 run


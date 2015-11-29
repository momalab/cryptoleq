#!/bin/sh

build2()
{
cd ..
echo building $1 $2 $3
make clean 
make CELLDEF=$2 MEMDEF=$3 PLAT=$1
make CELLDEF=$2 MEMDEF=$3 PLAT=$1
cd tests
}

testrun()
{
echo running tests $1 $2 $3
sh ../scop.sh PLAT $1 clean
sh ../scop.sh PLAT $1 all
sh ../scop.sh PLAT $1 html z_$1$2$3.html
}

buildrun()
{
build2 $1 $2 $3
testrun $1 $2 $3
cat _copres/_stat_cases | grep -v OK >> fails
}

run2()
{
buildrun $1 1 $2
buildrun $1 2 $2
buildrun $1 3 $2
buildrun $1 4 $2
#buildrun $1 5 $2
}

run()
{
run2 $1 1
run2 $1 2
run2 $1 3
}

: > fails
run msc
run unx



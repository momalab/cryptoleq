#!/bin/sh

tm=`date +%y%m%d%H%M%S`
echo $tm

make clean

cd ../..
fcl3 make -D cryptoleq cryptoleq$tm
bzip2 cryptoleq$tm.fcl
cd cryptoleq


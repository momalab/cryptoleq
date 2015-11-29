#!/bin/sh

exe=../../_bin_unx/ceal
#exe=../zeal/ceal_unx_c4_m3.exe

$exe 204n.sca -p "r=1" -c xenc db.opn -o db.sec
# set r=1 for input db - so when comparing key and input
# they are compared as open values - otherwise
# it is impossible to compare (while they are
# encrypted) because the result is random
dos2unix db.sec
cat db.sec | tr \\12 \\40 | gawk '{ print $7 }' > input.sec
#dos2unix input.sec
time -p $exe 204n.sca -d stat.n.txt
#rm db.sec input.sec
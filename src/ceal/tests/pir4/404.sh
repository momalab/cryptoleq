#!/bin/sh

exe=../../_bin_unx/ceal
#exe=../zeal/ceal_unx_c4_m3.exe

$exe 404.sca -c xenc db.opn -o db.sec
$exe 404.sca -c xenc input.opn -o input.sec
dos2unix db.sec
dos2unix input.sec
time -p $exe 404.sca -e
time -p $exe 404.sce
#time -p $exe 404.sca -d stat

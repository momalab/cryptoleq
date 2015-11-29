#!/bin/sh

exe=./ceal_unx_c4_m3.exe

rm -f *.sec
$exe 204.scam -c xenc db.opn -o db.sec
$exe 204.scam -c xenc input.opn -o input.sec
dos2unix db.sec
dos2unix input.sec


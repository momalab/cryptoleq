set PL=msc
..\..\_bin_%PL%\ceal 204.sca -c xenc db.opn -o db.sec
..\..\_bin_%PL%\ceal 204.sca -c xenc input.opn -o input.sec
dos2unix db.sec
dos2unix input.sec
::start /HIGH sh -c "sh 204.sh & cat"

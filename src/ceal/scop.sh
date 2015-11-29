#!/bin/bash

#cop exit statuses
exit_ok=0
exit_no_params=1
exit_tests_fail=2
exit_error=3
exit_status=$exit_ok

uname -o | grep -i "Linux" > /dev/null && CURROS="Linux" || CURROS="Windows"

cwd="$(pwd)"
[ "z${CURROS}" = "zLinux" ] && cd "$(dirname "$0")" || cd "$(dirname "${BASH_SOURCE[0]}")"
mydir="$(pwd)"
vgsupp="${mydir}/bash.supp"
cd "$cwd"

vgpath="/opt/valgrind/bin"
if [ -f "${vgpath}/valgrind" ]
then
	vgapp="${vgpath}/valgrind"
else
	vgapp="valgrind"
fi
memcheck="$vgapp\
	--tool=memcheck \
	--leak-check=full \
	--show-leak-kinds=all \
	--trace-children=yes \
	--suppressions=$vgsupp \
	--gen-suppressions=all \
	--log-fd=1 \
	--quiet"

cop=cop
fcldir=_fcl
fcl=fcl3
fclext=stsfcl

if [ "z$1" = "zPLAT" ]
then
PLAT=$2
shift 2
fi

[ "z${CURROS}" = "zLinux" ] && PLAT=${PLAT:-unx} || PLAT=${PLAT:-msc}

execdir=_bin_${PLAT}

#------------------------------------------------
find_bin() {
	cd "$1"
	while :
	do
	ccwd="$(pwd)"
	#echo "Search for $execdir: [$ccwd]"
	if [ $ccwd = "/" ] ; then
		break
	fi
	if test -d $execdir; then
		CLEQSRC="$ccwd"
		break
	fi
	if test -d src/$execdir; then
		CLEQSRC="${ccwd}/src"
		break
	fi
	cd ..
	done
}

find_bin "$mydir"
[ -z "$CLEQSRC" ] && find_bin "$cwd"
[ -z "$CLEQSRC" ] && echo "Error: cannot find $execdir"
#------------------------------------------------

#CLEQSRC=`pwd`
cd $cwd

targfile=$CLEQSRC/$execdir/

execpath=$targfile

newlinefilter="tr -d \015"
: ${COPDIFF:=diff}


sts=.sts

ver1=sca
ver2=sct
ver3=sts
ver4=sts4
vers=

if [ "$SADI" = "" ]
then
:
else
echo unset SADI
exit
fi

if [ "$CLEQSRC" = "" ]
then
echo set CLEQSRC
exit
fi

if [ "$PLAT" = "" ]
then
echo set PLAT
exit
fi

getbasename(){
   [ -z "$1" ] || echo $(echo "$1"|sed 's/\.tmp$//'|sed 's/\.[a-zA-Z0-9]*$//')
#   [ -z "$1" ] || echo $(echo "$1"|sed 's/\.[a-zA-Z0-9]\{3\}$//')
}

unpackfcl(){
   if ls *.$fclext 1> /dev/null 2> /dev/null ; then
   mkdir -p $fcldir
   for i in *.$fclext
   do
	if test -d $i ; then
	:
	else
	cp $i $fcldir/
	cd $fcldir
	chmod 0777 $i
	$fcl extr $i
	rm $i
	cd ..
	fi
   done
   fi
}

getvers(){
vers=""
   if ls *.$ver1 1> /dev/null 2> /dev/null ; then
   for i in *.$ver1
   do
	if test -d $i ; then
	:
	else
	vers="$vers $i"
	fi
	
   done
   fi
   if ls *.$ver2 1> /dev/null 2> /dev/null ; then
   for i in *.$ver2
   do
	if test -d $i ; then
	:
	else
	vers="$vers $i"
	fi
	
   done
   fi
   if ls *.$ver3 1> /dev/null 2> /dev/null ; then
   for i in *.$ver3
   do
	if test -d $i ; then
	:
	else
	vers="$vers $i"
	fi
	
   done
   fi
   if ls *.$ver4 1> /dev/null 2> /dev/null ; then
   for i in *.$ver4
   do
	if test -d $i ; then
	:
	else
	vers="$vers $i"
	fi
	
   done
   fi
}


nmstrt=all-
fp=
comm=$1
dirc=`pwd`
n=

failed_cases=_failed_cases
stat_cases=_stat_cases
failed_list=_failed_list
failed_dir=_failed_dir
touchdir=_copt
resdir=_copres
timeru=_timeru
HOST=`hostname`

#html_cases=cases.html

failfile=$dirc/$resdir/$failed_cases
listfile=$dirc/$resdir/$failed_list
timefile=$dirc/$resdir/$timeru
timeruloc=/home/omazonka/scr/timeru

mkdir -p $dirc/$resdir

stat_tot=10000
stat_fail=10000
stat_file=$dirc/$resdir/$stat_cases
# echo "0PID	Total	Filed	Status	Name" > $stat_file

nbase=
ndir=

cutoffx(){
     cat $1 | $newlinefilter | grep -v "HOST:\|Technology Pty Ltd" | \
	$2 > $1.tmp
}

#cutoffx()
#{
#	cat $1 | $newlinefilter | sed -e 's/{[^}]*}/{\.\.\.}/g' | \
#	grep -x -v "{\.\.\.}" | $2 > $1.tmp
#}

rnddir=
getrnddir(){
rnddir=""
fls=""

   for i in *
   do
    echo aaa > /dev/null &
    abbr=$!
    abbr=`expr $abbr % 7 % 2`
    if [ "$abbr" = "1" ]; then
    fls="$fls $i"
    else
    fls="$i $fls"
    fi
   done

rnddir=$fls

# uncomment this to see the random order of subdirs
#echo rnddir=$fls
}

recdel(){
if test -d "$1"; then

   if test -h $1; then
	return
   fi

   if test -x $1; then
	:
   else
	return
   fi

   cd $1
   echo -n "($1) " 

   getvers
   if [ "$vers" = "" ]
   then
   :
   else
   for i in $vers
   do
    n=$(getbasename $i)

      test -f $n.sce && rm -f $n.sce && echo -n "($n.sce) "
      test -f $n.out && rm -f $n.out && echo -n "($n.out) "
#err      test -f $n.err && rm -f $n.err && echo -n "($n.err) "
      test -f $n.out.tmp && rm -f $n.out.tmp && echo -n "($n.out.tmp ) "
#err      test -f $n.err.tmp && rm -f $n.err.tmp && echo -n "($n.err.tmp ) "
     
   done
   fi

   # do not remove gold
   #test -d .gold && rm -rf .gold && echo removing $1/.gold
   # instead
#   test -d .gold && rm -rf .gold/*.tmp
   rm -f *.gold.tmp
   # remove logs
   rm -f *.log

   # /// remove the line below
   test -d .coptouchdir && rm -rf .coptouchdir && echo -n " $1"

   test -d $fcldir && rm -rf $fcldir && echo -n " $1"
   test -d $touchdir && rm -rf $touchdir && echo -n " $1"
   test -d $resdir && rm -rf $resdir && echo -n " $1"
   test -f $failed_cases && rm -f $failed_cases && \
      echo removing $1/$failed_cases
   test -f $timeru && rm -f $timeru && \
      echo removing $1/$timeru
   test -f $failed_list && rm -f $failed_list && \
      echo removing $1/$failed_list
   test -f $stat_cases && rm -f $stat_cases && \
      echo removing $1/$stat_cases

   #test -f $html_cases && rm -f $html_cases &&  echo removing $1/$html_cases

   test -d $failed_dir && rm -rf $failed_dir && \
      echo removing $1/$failed_dir
   for i in *
   do
    recdel "$i"
   done
   cd ..
fi
}

rectouch(){
if test -d "$1"; then

   if test -h "$1"; then
	return
   fi
   if test -x "$1"; then
	:
   else
	return
   fi

   cd "$1"

   for i in *
   do
    rectouch "$i"
   done
#   test -d $touchdir && rm -rf $touchdir && echo -n " $1"
   test -d $touchdir && rm -rf $touchdir && echo -n "."

   cd ..
fi
}

runcase(){

   n=$(getbasename $1)
   if [ "$comm" = "updateall" ]
   then
      test -f $n.out && mv $n.out $n.gold && \
      rm -f $n.out.tmp && \
      echo updating $2$n.out
#err      test -f $n.err && mv $n.err .gold/$n.err && \
#err      rm -f $n.err.tmp && \
#err      echo updating $2$n.err
   else

    echo -n "$2$n: "

     if [ "$comm" = "reassess" ]
     then
      echo -n "reassessing "
      if [ ! -f $n.out ] ; then
      test -f $n.gold && cp $n.out $n.gold && \
      echo -n "skipping $2$n.out "
      fi
#      if [ -f $n.err ] ; then
#	:
#      else
#      test -f .gold/$n.err && cp .gold/$n.err $n.err && \
#      echo -n "skipping $2$n.err "
#      fi
     else

h1=`date +'%H'`
m1=`date +'%M'`
s1=`date +'%S'`
t1=`expr $h1 \* 3600 + $m1 \* 60 + $s1`

#set SADIPATH=$execpath
if [ "$comm" = "memcheck" -o "$comm" = "runmemcheck" ]
then
	cmd="$memcheck sh $n$sts $execpath"
else
	cmd="sh $n$sts $execpath"
fi

rm -f $n.sce

fullfile=$1
extn=${fullfile##*.}

if [ "$extn" = "sts" ]
then
sh $1 $execpath 1> $n.out
else
######################################
# COMMAND                            #
######################################
cmd="$execpath/ceal $1 -o stdout"  #
$cmd 1> $n.out                       #
######################################
fi

s2=`date +'%S'`
m2=`date +'%M'`
h2=`date +'%H'`
t2=`expr $h2 \* 3600 + $m2 \* 60 + $s2`
t3=`expr $t2 - $t1`

# uncomment this line to produce timeru
# echo "$t3   $2$n" >> $timefile

     fi

# Do the same as for err
	flaglist=noprint

	stat_verd=OK
	faildir=$dirc/$resdir/$failed_dir/$PLAT/$2

	cutoffx $n.out cat
	if test -f $n.gold
	then
		cutoffx $n.gold cat
	fi
	if cmp -s $n.gold.tmp $n.out.tmp
	then
		echo -n "out "
		rm -f $n.out $n.sce
		rm -f $n.out.tmp
		rm -f $n.gold.tmp
	else
		exit_status=$exit_tests_fail
		#    echo -n "[$2$n.out] "
		echo ""
		echo "****** [$2$n.out] ******* FAILED"
		echo $cop diff $2$n.out.tmp >> $failfile
		echo $2$n$sts >> $listfile
		flaglist=yes
		stat_verd=FAIL
		mkdir -p $faildir
		cp $n.out.tmp $faildir/
		cp $n.out $faildir/
    		if test -f $n.gold.tmp
		then
			if test -f /usr/local/bin/diffh; then	
				diff -u $n.gold.tmp $n.out.tmp | diffh > $faildir/$n.diff.out.tmp
				diff -u $n.gold $n.out | diffh > $faildir/$n.diff.out
			else
				sdiff $n.gold.tmp $n.out.tmp > $faildir/$n.diff.out.tmp
				sdiff --strip-trailing-cr $n.gold $n.out > $faildir/$n.diff.out
			fi
		else
			echo "Check filtered output and cop update when satisfied"
		fi
	fi 
#err    cutoffx $n.err sort
#err    if test -f .gold/$n.err; then
#err    cutoffx .gold/$n.err sort
#err    fi
#err    if cmp -s .gold/$n.err.tmp $n.err.tmp; then
#err    echo -n "err "
#err    rm -f $n.err
#err    rm -f $n.err.tmp
#err    rm -f .gold/$n.err.tmp
#err    else
#err    echo -n "[$2$n.err] "
#err    echo $cop diff $2$n.err.tmp >> $failfile
#err      if [ "$flaglist" = "noprint" ]; then
#err      echo $2$n >> $listfile
#err      fi
#err    stat_verd=FAIL
#err	mkdir -p $faildir
#err	cp $n.err.tmp $faildir/
#err	cp $n.err $faildir/
#err   		if test -f .gold/$n.err.tmp; then
#err			sdiff .gold/$n.err.tmp $n.out.tmp > $faildir/$n.diff.err.tmp
#err			sdiff --strip-trailing-cr .gold/$n.err $n.out > $faildir/$n.diff.err
#err		else
#err			echo "Check filtered error output and cop update when satisfied"
#err		fi
#err    fi

    stat_tot=`expr $stat_tot + 1`
    if [ "$stat_verd" != "OK" ]; then
	stat_fail=`expr $stat_fail + 1`
    fi
    echo "$PLAT	$$	$stat_tot	$stat_fail	$stat_verd	$2$n" >> $stat_file

   fi  # update
}

func(){
if test -d $1; then

   if test -h $1; then
	return
   fi
   if test -x $1; then
	:
   else
	return
   fi

   cd $1
   if test -d $touchdir; then
	cd ..
	return
   fi

   unpackfcl

   getrnddir
   for i in $rnddir
   do
    func "$i" "$2$i/"
   done

   if mkdir $touchdir 1> /dev/null 2> /dev/null
   then
     getvers
     if [ "$vers" = "" ]
     then
     :
     else
     :
#    mkdir -p .gold

     for i in $vers
     do
     runcase $i $2
     done
     echo "($1) "
     fi
   fi
   cd ..
#else 
#  nothing to do
fi
}


help1(){
	echo "If <execpath> is not specified then the file used is"
	echo "'$targfile'"
	echo "The script tries to compare the results with"
	echo "previously stored results. If there are no stored"
	echo "results, it will report test cases as failed."
	echo "To store results use the commands 'update' or 'updateall'."
}

error() {
	[ -z "$1" ] || echo "$1"
	exit $exit_error
}

case "$1" in
"all"|"memcheck")
	[ -d $targfile ] || error "$targfile (default) not accessible"

	[ -z "$2" ] || targfile=$2

	[ -d $targfile ] || error "no $targfile"

	echo echo Started $$ $PLAT on $HOST in $dirc: `date`>> $failfile
	func . ${fp}
	echo echo Finished $$ [`times | tr '\n' ' '`]: `date`>> $failfile
	;;
"updateall")
	echo echo updating all: `date`>> $failfile
	func . ${fp}
	;;
"reassess")
	echo echo Reassessing >> $failfile
	echo echo Failed cases: >> $failfile
	func . ${fp}
	echo echo Finished: `date`>> $failfile
	cat $failfile
	;;
"clean")
	echo "Cleaning all temporary files"
	recdel .
	echo " "
	;;
"reset")
	echo "Cleaning $touchdir"
	rectouch .
	echo " "
	;;
"update")
	n=$(getbasename $2)
	if test -f $n.out || test -f $n.err
	then
		echo "updating $n"
		echo echo updating $n: `date`>> $failfile
		#   mkdir -p .gold
		nbase=`basename $n`
		ndir=`dirname $n`
		#echo "NBASE: $nbase"
		#echo "NDIR: $ndir"
		test -f $n.out && mv $n.out $ndir/$nbase.gold && rm -f $n.out.tmp
		#err   test -f $2.err && mv $2.err $ndir/.gold/$nbase.err && rm -f $2.err.tmp
	else
		echo "no such failed test case (use the name without .out, or .err)"
	fi
	;;
"show")
	echo $failfile
	test -f $failfile && cat $failfile || echo "No test results to display (clean was run)"
	;;
"rnd")
	echo $$
	;;
"diff")
	[ -z "$2" ] && error "Please read help, i need 2nd argument"
	nbase=$(getbasename $2)
	ndir=$(dirname $2)
	echo $COPDIFF $ndir/${nbase}.gold.tmp $ndir/${nbase}.out.tmp
	#  $COPDIFF $ndir/${nbase}.gold.tmp $ndir/${nbase}.out.tmp
	$COPDIFF ${nbase}.gold.tmp ${nbase}.out.tmp
	;;
"run"|"runmemcheck")
	[ -z "$2" ] && error "no test case specified"
	[ -f "$2" ] || error "no test file $2"

	nbase=`basename $2`
	ndir=`dirname $2`

	[ -z "$3" ] || targfile=$3

	[ -d $targfile ] || error "no file $targfile"

	echo echo run $2: `date`>> $failfile
	cd $ndir
	runcase $nbase "$ndir/"
	cd $dirc
	;;
"diffall")
	while read LINE
	do
		echo $LINE
		$LINE
	done < $failfile
	;;
"html")
	if [ "$2" = "" ]
	then
		htmlout=$resdir/z.html
		relp=../
	else
		htmlout=$2
		relp=./
	fi

	: > $htmlout

	echo "<html><body>" >> $htmlout
	echo '<meta http-equiv="Content-Type" content="text/html; charset=utf-8">' >> $htmlout
	echo "<h3>Generated $PLAT on $HOST in [$dirc]</h3>" >> $htmlout
	echo "<h4>" `date` "</h4>" >> $htmlout
	echo "<pre>" >> $htmlout
	test -f $resdir/$failed_list && cat $resdir/$failed_list >> $htmlout

	echo "</pre><table border=1 cellspacing='0' cellpadding='1' \
	bordercolor='#CC9966' style='border-color:#119966;border-width:1px;\
	border-style:solid;border-collapse:collapse;'>"  >> $htmlout

	echo "<tr><th>PLAT</th><th>PID</th><th>Total</th><th>Failed</th>" >> $htmlout
	echo "<th>Status</th><th>Name</th></tr>" >> $htmlout

	sort $stat_file > $stat_file.tmp

	while read PLT PID TOT FTOT STAT NAME
	do
		if [ "$STAT" != "OK" ]; then
			REF=$resdir/$failed_dir/$PLT/$NAME
			REF1=$REF.diff.out.tmp
			REF2=$REF.diff.err.tmp
			REF3=$REF.diff.out
			REF4=$REF.diff.err

			if test -f $REF1; then REF1="<a href='$relp$REF1'>F</a>"; else REF1=F; fi
			if test -f $REF2; then REF2="<a href='$relp$REF2'>A</a>"; else REF2=A; fi
			if test -f $REF3; then REF3="<a href='$relp$REF3'>I</a>"; else REF3=I; fi
			if test -f $REF4; then REF4="<a href='$relp$REF4'>L</a>"; else REF4=L; fi

			STAT="<font color=red size=4><b>$REF1 $REF2 $REF3 $REF4</b></font>"

		fi
		echo "<tr>" >> $htmlout
		echo "<td>$PLT</td><td>$PID</td><td>$TOT</td><td>$FTOT</td><td>$STAT</td><td>$NAME</td>" >> $htmlout
		echo "</tr>" >> $htmlout
	done < $stat_file.tmp

	rm -f $stat_file.tmp

	echo "</table></body></html>" >> $htmlout
	exit	
	;;
"runlist")
	[ -z "$2" ] && error "no listfile specified"
	[ -f "$2" ] || error "no file $2"
	[ -z "$3" ] || targfile=$3
	[ -f "$targfile" ] || error "no file $targfile"

	echo echo Started list $2: `date`>> $failfile
	echo echo Failed cases: >> $failfile

	listfile=$2
	while read LINE
	do
		if test -f $LINE; then
			nbase=`basename $LINE`
			ndir=`dirname $LINE`
			echo running $LINE
			cd $ndir
			runcase $nbase "$ndir/"
			cd $dirc
		else
			echo no file $LINE
			echo echo no file $LINE >> $failfile
		fi
	done < $listfile

	echo echo Finished: `date`>> $failfile
	cat $failfile
	;;
"")
	echo "Use commands:	help [command], all [execpath], memcheck [execpath],"
	echo "		show, updateall, update <case>, diff <case>, clean, reassess,"
	echo "		run <case>  [execpath], runmemcheck <case>  [execpath],"
	echo "		runlist <listfile>  [execpath], reset, rnd, html, diffall."
	echo "		NOTE: PLAT can be 2 first arguments; e.g. cop PLAT unx all"
	exit $exit_no_params
	;;
"help")
	case "$2" in

	"help"|"")
		echo "Select any command and try help <command>"
		echo "For example, help show"
		echo "Commands available:"
		echo "	all [execpath]"
		echo "	clean"
		echo "	diffall"
		echo "	diff <case>.{out|err}[.tmp]"
		echo "	help [command]"
		echo "	html"
		echo "  memcheck [execpath]"
		echo "	reassess"
		echo "	run <case> [execpath]"
		echo "  runmemcheck <case> [execpath]"
		echo "	runlist <listfile> [execpath]"
		echo "	reset"
		echo "	show"
		echo "	updateall"
		echo "	update <case>"
		;;
	"all")
		echo "	all [execpath]"
		echo "Use command 'all' to run all files starting from"
		echo "current directory to all recursive subdirectories."
		help1
		;;
	"memcheck")
        	echo "  memcheck [execpath]"
		echo "Use command 'memcheck' to run all files starting from"
		echo "current directory to all recursive subdirectories."
		echo "All tests passed througth valgrind memcheck filter."
		;;
	"clean")
		echo "	clean"
		echo "Removes recursively all temp files produced by the script."
		;;
	"reset")
		echo "	reset"
		echo "Removes recursively all $touchdir directories produced by the script."
		;;
	"diff")
		echo "	diff <case>.{out|err}[.tmp]"
		echo "Runs the command to compare the obtained result"
		echo "against the stored result."
		echo "The argument is an output file which can be .out, .err,"
		echo "or .tmp (stripped) file."
		echo "For example:"
		echo "	diff aaa/bbb/fff.out"
		echo "	diff aaa/bbb/fff.err.tmp"
		;;
	"reassess")
		echo "	reassess"
		echo "This command does not run scripts."
		echo "It goes recursively in search for .out and .err files,"
		echo "strips them to .tmp and compares to the stored output."
		echo "This command is useful only when you do not want to rerun"
		echo "again all the tests, but you want to compare results under"
		echo "different conditions (you changed the stripping function)."
		echo "[In this script (line ~40) there is the stripping function"
		echo "'cutoffx' which greps out volatile lines]. The other case"
		echo "you might want to use this command is creating a list of"
		echo "the failed cases on a subset of test cases (see 'help show')."
		;;
	"run")
		echo "	run <case> [execpath]"
		echo "This command runs one selected script."
		echo "For example:"
		echo "	run aaa/bbb/fff.$ver1"
		echo "	run aaa/bbb/${ver1}file.my prototype"
		help1
		echo "It does not check file for the proper extension,"
		echo "so another axtension can be used."
		;;
	"runmemcheck")
		echo "	runmemcheck <case> [execpath]"
		echo "This command runs one selected script"
		echo "with valgrind memcheck filter."
		echo "For example:"
		echo "	runmemcheck aaa/bbb/fff.$ver1"
		echo "	runmemcheck aaa/bbb/${ver1}file.my prototype"
		help1
		echo "It does not check file for the proper extension,"
		echo "so another axtension can be used."
		;;
	"runlist")
		echo "	runlist <listfile> [execpath]"
		echo "This command is almost the same as 'run', except that"
		echo "it reads <listfile> and runs every script in the list."
		echo "For example:"
		echo "	runlist list /tmp/prototype"
		echo "	where 'list' is a file like:"
		echo "		aaa/bbb/fff_1.vsc"
		echo "		aaa/bbb/fff_2.vsc"
		echo "		aaa/fff.vsc"
		;;
	"show")
		echo "	show"
		echo "This command shows the failed test cases."
		echo "The failed cases can be show only from the"
		echo "directory where the script was run from."
		echo "If you want to see the failed cases from"
		echo "another directory (different subset of test cases),"
		echo "then make first 'reassess' from that directory."
		echo "[The format is ready to copy and paste.]"
		;;
	"updateall")
		echo "	updateall"
		echo "This command stores all the outputs."
		echo "Previous stored outputs are erased."
		echo "The stored outputs are compared against"
		echo "when you run the script again later on."
		;;
	"update")
		echo "	update <case>"
		echo "Updates one test case."
		echo "<case> is a name of the file with the"
		echo "relative path" 
		echo "For example:"
		echo "	update aaa/bbb/fff.$ver1"
		echo "	where the real file is aaa/bbb/fff.$ver1"
		;;
	"html")
		echo "	html [file]"
		echo "This command generates html file with results."
		echo "If file argument is absent the output is stored in $resdir/z.html"
		;;
	"diffall")
		echo "	html"
		echo "Run diff for all failed test cases."
		;;
	*)
	 	echo no such command: $2
	esac
	;;
*)
	echo "try help"
esac

exit $exit_status

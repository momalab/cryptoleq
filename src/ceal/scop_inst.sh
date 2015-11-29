#!/bin/sh

ad=${ad:-c:/ap14}

ap=`cygpath $ad`/run
co=`pwd`/scop.sh

if test -d $ap
then
    :
else
    echo "cannot find valid directory $ad"
    echo "try command: env ad=<DIR> sh scop_inst.sh"
    echo "where <DIR> is your ap install directory"
    echo "for example:"
    echo "env ad=c:/ap14 sh scop_inst.sh"
    exit
fi


if [[ :$PATH: == *:"$ap":* ]] ; then
    :
else
    echo "directory $ap is not on the \$PATH"
    echo "check that directory $ad is your current working"
    exit
fi

if test -f $ap/scop.bat
then
    echo "$ad/run/scop.bat - is installed"
    echo "if you want to reinstall cop, remove file (rm $ad/run/scop.bat) and run again"
    exit
fi

echo "@sh $co %*" > scop.bat
mv scop.bat $ap/

echo "call scop run %1" > scopfix.bat
echo "call scop update %1" >> scopfix.bat
echo "call scop run %1" >> scopfix.bat
mv scopfix.bat $ap/

echo "installing scop in $ap"


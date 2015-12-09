
COMPILER=cl
COMPILERC=cl
AR=arXXX
ARCR=lib /OUT:
LD=ld
RANLIB=ranlibXXX
STRIP=strip

#OPT=$(OPTNOWRN) /WX -W4 -nologo /EHsc /Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702
#OPT=$(OPTNOWRN) /WX -W4 -nologo /EHsc /Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702 /wd4100
#OPT=$(OPTNOWRN) /WX -W4 -nologo /EHsc -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1
OPT=/WX -W4 -nologo /EHsc -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1
OPT=-WX -W4 -nologo -EHsc -Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1
OPT=-WX -nologo -EHsc -Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1
OPT=-WX -nologo -EHsc -Ox -D_CRT_SECURE_NO_DEPRECATE=1 -D_USING_V110_SDK71_=1

# this option gives one antivirus to report
OPT=-WX -nologo -EHsc -Ox 

# this tests fine on virustotal.com 151209
OPT=-WX -nologo -EHsc -Og -O2

#4127 conditional expression is constant
#4244 possible loss of data
#4100 unreferenced formal parameter
#4309 trucation of constant value
#4702 unreachable code
#4290 C++ exception specification ignored except to indicate a function is not __declspec(nothrow)

OPTC=$(OPT)

INC=
EEXT=.exe
OEXT=.obj
LEXT=.lib
DEFEXT=.exe
LDF=WS2_32.Lib
OOUT=-Fo
EOUT=-Fe


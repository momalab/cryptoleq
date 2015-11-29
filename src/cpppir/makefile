
all: prog_sec.exe crypter.exe

CL=cl
CLC=cl
OPT=-WX -W4 -nologo -EHsc -Ox -D_CRT_SECURE_NO_DEPRECATE=1 /wd4127 /wd4702 /wd4100 /wd4290 -D_USING_V110_SDK71_=1
o=obj
EOUT=-Fe

ifdef gcc
CL=g++
CLC=gcc
OPT=-std=c++11 -O2
o=o
EOUT=-o
endif

ifdef GMP
OPT := -DGMP=1 $(OPT)
LDF := -lgmpxx -lgmp $(LDF)
endif

prog_sec.exe: prog_sec.cpp secure_int.$o *.h secure_ptr.$o \
	open_int.$o base_int.$o unumberg.$o cunmber_4096_m.$o compiler.$o \
	ma_invert_m.$o secure_mul.$o secure_mdl.$o open_mul.$o \
	processor.$o memcell.$o
	$(CL) $(OPT) prog_sec.cpp secure_int.$o secure_ptr.$o \
	open_int.$o base_int.$o unumberg.$o cunmber_4096_m.$o compiler.$o \
	ma_invert_m.$o secure_mul.$o secure_mdl.$o open_mul.$o \
	processor.$o memcell.$o $(LDF) \
	$(EOUT)$@

crypter.exe: crypter.cpp secure_int.$o *.h secure_ptr.$o \
	open_int.$o base_int.$o unumberg.$o cunmber_4096_m.$o compiler.$o \
	ma_invert_m.$o secure_mul.$o secure_mdl.$o open_mul.$o \
	processor.$o memcell.$o
	$(CL) $(OPT) crypter.cpp secure_int.$o secure_ptr.$o \
	open_int.$o base_int.$o unumberg.$o cunmber_4096_m.$o compiler.$o \
	ma_invert_m.$o secure_mul.$o secure_mdl.$o open_mul.$o \
	processor.$o memcell.$o $(LDF) \
	$(EOUT)$@

secure_int.$o: secure_int.cpp secure_int.h
	$(CL) $(OPT) -c secure_int.cpp

secure_ptr.$o: secure_ptr.cpp secure_ptr.h
	$(CL) $(OPT) -c secure_ptr.cpp

open_int.$o: open_int.cpp open_int.h
	$(CL) $(OPT) -c open_int.cpp

base_int.$o: base_int.cpp base_int.h
	$(CL) $(OPT) -c base_int.cpp

cunmber_4096_m.$o: ../unumber/cunmber_4096_m.c
	$(CLC) $(OPT) -c $<

unumberg.$o: ../unumber/unumberg.cpp
	$(CL) $(OPT) -c $<

compiler.$o: ../processor/compiler.cpp ../processor/compiler.h
	$(CL) $(OPT) -c $<

ma_invert_m.$o: ../unumber/ma_invert_m.cpp
	$(CL) $(OPT) -c $<

secure_mul.$o: secure_mul.cpp secure_int.h
	$(CL) $(OPT) -c secure_mul.cpp

secure_mdl.$o: secure_mdl.cpp secure_int.h
	$(CL) $(OPT) -c secure_mdl.cpp

open_mul.$o: open_mul.cpp open_int.h
	$(CL) $(OPT) -c open_mul.cpp

processor.$o: ../processor/processor.cpp ../processor/processor.h
	$(CL) $(OPT) -c $<

memcell.$o: ../processor/memcell.cpp ../processor/memcell.h
	$(CL) $(OPT) -c $<

clean:
	rm -f *.obj *.exe *.o
	rm -rf output.opn db.sec input.sec output.sec
	cd vc; make clean


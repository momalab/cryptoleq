# Orthfun research
# New York University at Abu Dhabi, MoMa Lab
# Copyright 2014-2015
# Author: Oleg Mazonka


MYLIB=sqclib

SRC=./
include $(SRC)mk_all.mak

#CELLDEF=1 - defaulted in memcell.h
ifdef CELLDEF
OPT := -DCELLDEF=$(CELLDEF) $(OPT)
endif

#MEMDEF=1 - defaulted in memory.h
ifdef MEMDEF
OPT := -DMEMDEF=$(MEMDEF) $(OPT)
endif

ifdef GMP
OPT := -DGMP=1 $(OPT)
LDF := -lgmpxx -lgmp $(LDF)
endif

O=$(TRG)

src = 

obj := $(src:%.cpp=$O/%$(OEXT))

src2 = compiler.cpp processor.cpp memcell.cpp
obj2 := $(src2:%.cpp=$O/%$(OEXT))

srctrg = orth01.cpp orth02.cpp orth03.cpp orth04.cpp \
        orth05.cpp orth06.cpp
trg := $(srctrg:%.cpp=$O/%$(EEXT))


all: $O $(trg) $O/$(MYLIB)$(LEXT)

$O:
	mkdir -p $O

$(trg): $O/%$(EEXT):%.cpp $O/$(MYLIB)$(LEXT) $O/unumberg$(OEXT)\
	$O/cunmber_4096_m$(OEXT) $O/ma_invert_m$(OEXT)
	$(COMPILER) $(OPT) $(INC) $< $O/$(MYLIB)$(LEXT) \
	$O/unumberg$(OEXT) $O/cunmber_4096_m$(OEXT) \
        $O/ma_invert_m$(OEXT)  $(LDF) $(EOUT)$@
#	rm -f *.obj

$(obj): $O/%$(OEXT):%.cpp *.h
	$(COMPILER) -c $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@

$(obj2): $O/%$(OEXT):../processor/%.cpp ../processor/*.h
	$(COMPILER) -c $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@

clean:
	rm -rf _bin* _copres
	rm -rf *.obj *.pdb *.gcno *.gcov *.gcda

$O/$(MYLIB)$(LEXT): $(obj) $(obj2)
	$(ARCR)$O/$(MYLIB)$(LEXT) $(obj) $(obj2)

$O/cunmber_4096_m$(OEXT): ../unumber/cunmber_4096_m.c
	$(COMPILERC) -c $(OPTC) $(INC) $(FLAGS) $< $(OOUT)$@

$O/ma_invert_m$(OEXT): ../unumber/ma_invert_m.cpp
	$(COMPILER) -c $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@

$O/unumberg$(OEXT): ../unumber/unumberg.cpp ../unumber/unumberz.cpp
	$(COMPILER) -c $(OPT) $(INC) $(FLAGS) $< $(OOUT)$@

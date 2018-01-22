# PLAT: w u

include mk_all.mak

CUD=c30
CULIB=cuddlib

O=$(TRG)

csrc = ut.c

obj := $($(CUD)/src:%.cpp=$O/%$(OEXT))

srctrg = main.cpp
trg := $(srctrg:%.cpp=%$(EEXT))


all: $O $(trg) $O/$(CULIB)$(LEXT)

$O:
	mkdir -p $O

$(trg): %$(EEXT):%.cpp $O/$(CULIB)$(LEXT) $O/cuddObj$(OEXT)
	$(COMPILER) $(OPT) $(INC) $< $O/$(CULIB)$(LEXT) \
	$O/cuddObj$(OEXT) $(LDF) $(EOUT)$@

$O/$(CULIB)$(LEXT): $(obj)
	$(ARCR)$O/$(CULIB)$(LEXT) $(obj)

$(obj): $O/%$(OEXT):%.cpp *.h
	$(COMPILER) -c $(OPT) $(OPTCD) $(INC) $(FLAGS) $< $(OOUT)$@

$O/cuddObj$(OEXT): $(CUD)/cuddObj.cc
	$(COMPILER) -c $(OPT) $(OPTCD) $(INC) $(FLAGS) $< $(OOUT)$@

# default values

ifeq ($(OS),Windows_NT)
PLAT=msc
else
PLAT=unx
endif

BINR=$(PLAT)
TRG=_build_$(BINR)
TRG=_bin_$(BINR)

include $(SRC)mk_$(PLAT).mak

ifdef MEMORY
OPT+= -DTEST_MEMORY=1
endif

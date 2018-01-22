# Hasq Technology Pty Ltd (C) 2013-2016

ifeq ($(OS),Windows_NT)
PLAT=w
else
PLAT=u
endif

BINR=$(PLAT)
TRG=_bin$(BINR)

include $(SRC)mk_$(PLAT).mak


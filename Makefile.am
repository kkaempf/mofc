# $Id: Makefile.am,v 1.1 2005/03/03 09:08:47 mihajlov Exp $

AM_LFLAGS=-i
AM_YFLAGS=-d

BUILT_SOURCES=mofc.h

if TESTSCANNER
bin_PROGRAMS=testmoflex mofc

testmoflex_SOURCES=mofs.l hash.c
testmoflex_CPPFLAGS=-DSCANDEBUG
else
bin_PROGRAMS=mofc
endif

mofc_SOURCES=mofc.y mofs.l mofparse.c hash.h hash.c symtab.h symtab.c mofdecl.h backend.h 
EXTRA_mofc_SOURCES=backend_print.c backend_sfcb.c
mofc_DEPENDENCIES=mofc-@BACKEND_OBJ@
mofc_LDADD=mofc-@BACKEND_OBJ@ @BACKEND_LIBS@ @LEXLIB@
mofc_CPPFLAGS=@BACKEND_INCLUDES@

EXTRA_DIST=runtest.sh tests/test1.mof tests/include/test2.mof \
	tests/include/test3.mof 

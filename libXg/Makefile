#	Copyright (c) 1998 Lucent Technologies - All rights reserved.
#
#	Prototype Makefile for libXg
#
include ../config.mk

#	add -Iincludedir for any include directories that need to be searched
INCS=-I../include

#	set this if your X libraries are in different locations
#	or if you need extra libraries to load with X11 applications
XLIBS=-lXt

#	add name of library orderer - use ":" if none
RANLIB=:

#	add name of librarian
AR=ar

#	the name of the library
LIB=libXg.a

CFLAGS=$(STANDARDS) $(INCS) $(INCLUDES) -DUSE64BITS=$(USE64BITS)
CC?=c99

OBJS=	  arith.o balloc.o bitblt.o   border.o bscreenrect.o\
	  clipr.o   cursorset.o cursorswitch.o\
	   font.o gcs.o getrect.o gwin.o ldconvert.o latin1.o\
	menuhit.o    rdbitmap.o \
	rectclip.o rune.o  string.o strwidth.o texture.o\
	wrbitmap.o   xtbinit.o

all install:	$(LIB)
compile:	$(LIB)
test:	$(LIB) test.o
	$(CC) -o $@ $? $(LIB) $(XLIBS) -lm $(LDFLAGS)
	echo try running test
clean:
	rm -f *.o test *.a

nuke:	clean
	rm -f $(LIB)

$(LIB):	$(OBJS)
	$(AR) rv $(LIB) $(OBJS)
	$(RANLIB) $(LIB)

$(LIB)(%.o): %.o

$(OBJS):	../include/libg.h libgint.h ../include/libc.h

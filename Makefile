#
# Makefile for BtYacc.
#

# Edited for Debian GNU/Linux.
DESTDIR =

VERSION       = 3.0

DEST	      = .

HDRS	      = defs.h mstring.h

CFLAGS	      = -g -Wall -O2 -Wstrict-prototypes -Wmissing-prototypes
 
LDFLAGS	      = #-static

LIBS	      =

CC	      = gcc
LINKER	      = gcc
# LINKER      = cl
# CC	      = cl

MAKEFILE      = Makefile

OBJS	      = closure.o error.o lalr.o lr0.o main.o mkpar.o output.o	\
		mstring.o reader.o readskel.o skeleton.o symtab.o verbose.o warshall.o

PRINT	      = pr -f -l88

PROGRAM	      = btyacc

SRCS	      = closure.c error.c lalr.c lr0.c main.c mkpar.c output.c	\
		mstring.c reader.c readskel.c skeleton.c symtab.c verbose.c warshall.c

OTHERS	      = README README.BYACC \
		Makefile btyaccpa.ske push.skel empty.y skel2c manpage makefile.dos \
		skeleton.c

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		$(LINKER) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LIBS)

clean:;		rm -f $(OBJS)

clobber:;	rm -f $(OBJS) $(PROGRAM)

distclean:;	rm -f $(OBJS) $(PROGRAM) skeleton.c *.zip *.gz

depend:;	mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;		ctags -wx $(HDRS) $(SRCS)

install:	$(PROGRAM)
		install $(PROGRAM) $(DESTDIR)/usr/bin

oldinstall:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		install -s $(PROGRAM) $(DEST)

listing:;	$(PRINT) Makefile $(HDRS) $(SRCS) | lpr

lint:;		lint $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS)
		ctags $(HDRS) $(SRCS)

dist:		tar zip

tar:
		rm -f btyacc.tar btyacc.tar.gz
		tar cvf btyacc.tar $(OTHERS) $(SRCS) $(HDRS) test/*.y
		gzip -9 btyacc.tar
		mv btyacc.tar.gz btyacc-$(VERSION).tar.gz

zip:
		zip btyacc.zip $(OTHERS) $(SRCS) $(HDRS) test/*.y
		mv btyacc.zip btyacc-$(VERSION).zip

skeleton.c: btyaccpa.ske skel2c
		awk -f skel2c btyaccpa.ske >skeleton.c
		awk -f skel2c btyaccpa.ske > skeleton.c
 
etags TAGS:
		etags *.c *.h

btyacc.1:	btyacc.sgml
		docbook-to-man btyacc.sgml > btyacc.1


###
closure.o: defs.h
error.o: defs.h
lalr.o: defs.h
lr0.o: defs.h
main.o: defs.h
mkpar.o: defs.h
mstring.o: mstring.h
output.o: defs.h
reader.o: defs.h mstring.h
skeleton.o: defs.h
symtab.o: defs.h
verbose.o: defs.h
warshall.o: defs.h

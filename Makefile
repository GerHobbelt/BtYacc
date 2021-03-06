#
# Makefile for BtYacc.
#

# Edited for Debian GNU/Linux/MSys.
DESTDIR =

VERSION       = 3.0.5

DEST          = .

HDRS          = defs.h mstring.h

#CFLAGS       = -g -Wall -O2 -Wstrict-prototypes -Wmissing-prototypes
CFLAGS        = -ggdb -Wall -O0 -Wstrict-prototypes -Wmissing-prototypes

LDFLAGS       = #-static

LIBS          =

CC            = gcc
LINKER        = gcc
# LINKER      = cl
# CC          = cl

DOCBOOK2MAN   = docbook2man

MAKEFILE      = Makefile

OBJS          = closure.o error.o lalr.o log.o lr0.o main.o mkpar.o output.o    \
		mstring.o reader.o readskel.o skeleton.o push_skel.o symtab.o verbose.o warshall.o write.o

PRINT         = pr -f -l88

PROGRAM       = btyacc

SRCS          = closure.c error.c lalr.c log.c lr0.c main.c mkpar.c output.c    \
		mstring.c reader.c readskel.c skeleton.c push_skel.c symtab.c verbose.c warshall.c write.c

OTHERS        = README README.BYACC \
		Makefile btyaccpa.skeleton push.skeleton empty.y skel2c manpage makefile.dos \
		skeleton.c push_skel.c

all:        $(PROGRAM) btyacc.1

$(PROGRAM):     $(OBJS) $(LIBS)
		$(LINKER) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(LIBS)

clean:;     rm -f $(OBJS)

clobber:;   rm -f $(OBJS) $(PROGRAM) $(PROGRAM).exe

distclean:; rm -f $(OBJS) $(PROGRAM) $(PROGRAM).exe skeleton.c push_skel.c *.zip *.gz

depend:;    mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)

index:;     ctags -wx $(HDRS) $(SRCS)

install:    $(PROGRAM)
		install $(PROGRAM) $(DESTDIR)/usr/bin

oldinstall: $(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		install -s $(PROGRAM) $(DEST)

listing:;   $(PRINT) Makefile $(HDRS) $(SRCS) | lpr

lint:;      lint $(SRCS)

program:        $(PROGRAM)

tags:           $(HDRS) $(SRCS)
		ctags $(HDRS) $(SRCS)

dist:       tar zip

tar:
		rm -f btyacc.tar btyacc.tar.gz
		tar cvf btyacc.tar $(OTHERS) $(SRCS) $(HDRS) test/*.y
		gzip -9 btyacc.tar
		mv btyacc.tar.gz btyacc-$(VERSION).tar.gz

zip:
		zip btyacc.zip $(OTHERS) $(SRCS) $(HDRS) test/*.y
		mv btyacc.zip btyacc-$(VERSION).zip

skeleton.c: btyaccpa.skeleton skel2c
		awk -f skel2c btyaccpa.skeleton > skeleton.c

push_skel.c: push.skeleton skel2c
		awk -f skel2c push.skeleton > push_skel.c

etags TAGS:
		etags *.c *.h

btyacc.1:   btyacc.sgml
		$(DOCBOOK2MAN) --sgml btyacc.sgml


###
closure.o: defs.h
error.o: defs.h
lalr.o: defs.h
log.o: log.h
lr0.o: defs.h log.h
main.o: defs.h log.h
mkpar.o: defs.h log.h
mstring.o: mstring.h
output.o: defs.h write.h
reader.o: defs.h mstring.h write.h
skeleton.o: defs.h
push_skel.o: defs.h
symtab.o: defs.h
verbose.o: defs.h
warshall.o: defs.h
write.o: write.h

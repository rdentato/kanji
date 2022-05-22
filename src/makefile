
TOP=..

SKPGEN=../dist/bin/skp

CC=gcc
##CC=clang

OPT=-O2 -DDEBUG=DEBUG_TEST
#OPT=-O2 -Os -fdata-sections -ffunction-sections
#OPT+=-ggdb3 -O0

#PROFFLAGS=-pg

LNFLAGS= $(PROFFLAGS)
LNFLAGS+=  -Wl,--gc-sections

CFLAGS = $(PROFFLAGS) $(OPT) -Wall -I$(TOP)/src -I$(TOP)/dist/src

OBJS=kanji_asm.o kanji_common.o kanji_exe.o kanji.o
OP_HDRS=opcodes_str.h opcodes_tok.h

all: kanji

kanji: $(OP_HDRS) $(OBJS)
	$(CC) -o $@ $(OBJS) $(LNFLAGS) 

opcodes_str.h: opcodes.lst
	sort -u opcodes.lst | awk 'BEGIN { print "char *opcodes = " ; n=0 } \
	                                 { if (n>0) print "\" \""; n=n+1; printf "\"%s\"",$$0} \
	                             END { print ";"}' > opcodes_str.h

opcodes_tok.h: opcodes.lst
	sort -u opcodes.lst | awk 'BEGIN {n=1} \
	                                 {printf "#define TOK_%s 0x%02X\n",$$0,n; n=n+1 } \
	                             END {print "#define NUMOPCODES " n-1}' > opcodes_tok.h

clean:
	$(RM) *.o kanji kanji_parse.c kanji_parse.h kanji_parse.t $(OP_HDRS)

# © 2022 Remo Dentato (rdentato@gmail.com)
# ⚖ MIT (https://opensource.org/licenses/MIT)
   
TOP=..

SKPGEN=../deps/bin/skpgen

CC=gcc
#CC=clang

AR=ar -r

OPT=-O2 -DDEBUG=DEBUG_TEST
OPT+=-fdata-sections -ffunction-sections
#OPT+=-ggdb3 -O0

#PROFFLAGS=-pg

LNFLAGS= -s $(PROFFLAGS) -L. 
#LNFLAGS+=-s -Wl,--gc-sections

CFLAGS = $(PROFFLAGS) $(OPT) -Wall -I$(TOP)/src -I$(TOP)/deps/src

OBJS = syslib.o kanji_sys.o kanji_common.o kanji_exe.o kanji_expr.o
OP_HDRS=opcodes_str.h opcodes_tok.h

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all:  zbasc

zbasc: parser.o zbasc.o 
	$(CC) -o $@ zbasc.o parser.o $(LNFLAGS)  

parser.c: parser.skp
	$(SKPGEN) parser

parser.o: parser.c
	$(CC) $(CFLAGS) -c -o $@ $<

kanji: libkaj.a kanji.o
	$(CC) -o $@ kanji.o $(LNFLAGS) 

# kanji: $(OP_HDRS) $(OBJS) kanji_asm.o kanji.o
# 	$(CC) -o $@ $(OBJS) kanji_asm.o kanji.o $(LNFLAGS) 

libkaj.a: $(OP_HDRS) $(OBJS) kanji_asm.o 
	$(AR) $@ $(OBJS) kanji_asm.o

kanji_run: $(OP_HDRS) $(OBJS)
	$(CC) -o $@ $(OBJS) $(LNFLAGS) 

kanji_sys: kanji_sys.c
	$(CC) -DSYS_MAIN $(CFLAGS) -o $@ kanji_sys.c

opcodes_str.h: opcodes.lst
	sort -u opcodes.lst | awk 'BEGIN { print "char *opcodes = " ; n=0 } \
	                                 { if (n>0) print "\"\\0\""; n=n+1; printf "\"%s\"",$$0} \
	                             END { print ";" }' > opcodes_str.h

opcodes_tok.h: opcodes.lst
	sort -u opcodes.lst | awk 'BEGIN { n=1 } \
	                                 { printf "#define TOK_%s 0x%02X\n",$$0,n; n=n+1 } \
	                             END { print "#define NUMOPCODES " n-1 }' > opcodes_tok.h

syslib.c: kanji_sys syslib/*.c
	@rm -f kanji_syslib.c
	@echo "#include \"val.h\"" > syslib.tmp
	@for f in syslib/*.c; do \
	   echo $$f; \
	   echo "#line 1 \"$$f\"" >> syslib.tmp ; \
		 cat $$f >> syslib.tmp ; \
	done ;
	sed -n 's/^ *val_t *\([a-zA-Z0-9]*\)\.\([a-zA-Z0-9]*\) *(.*/\1.\2/p' syslib.tmp > syslib.fnc
	sed    's/^ *val_t *\([a-zA-Z0-9]*\)\.\([a-zA-Z0-9]*\) *(/val_t sys_\1_\2(/' syslib.tmp > syslib.c
	sed    's/\([a-zA-Z0-9]*\)\.\([a-zA-Z0-9]*\)/val_t sys_\1_\2(val_t);/' syslib.fnc > syslib_fnc.h
	./kanji_sys < syslib.fnc > syslib.tmp
	sort syslib.tmp > syslib_lst.h
	rm -f syslib.tmp syslib.fnc

clean:
	$(RM) *.o kajc parser.c parser.h



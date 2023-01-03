# © 2022 Remo Dentato (rdentato@gmail.com)
# ⚖ MIT (https://opensource.org/licenses/MIT)
   
TOP=..

DEPSDIR=$(TOP)/deps
SKPGEN=$(DEPSDIR)/bin/skpgen
DEPSBIN=$(DEPSDIR)/bin/BINARIES

CC=gcc
#CC=clang

AR=ar -r
RM=rm -f

OPT=-O2 -DDEBUG=DEBUG_TEST
#OPT+=-fdata-sections -ffunction-sections
#OPT+=-ggdb3 -O0

#PROFFLAGS=-pg

LNFLAGS= -s $(PROFFLAGS) -L. 
#LNFLAGS+=-s -Wl,--gc-sections

CFLAGS = $(PROFFLAGS) $(OPT) -Wall -I$(TOP)/src -I$(DEPSDIR)/src

OBJS = syslib.o kanji_sys.o kanji_common.o kanji_exe.o kanji_expr.o
OP_HDRS=opcodes_str.h opcodes_tok.h

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

all:  kajc

kajc: fname.o parser.o kajc.o 
	$(CC) -o $@ fname.o parser.o kajc.o $(LNFLAGS)  

parser.c: $(SKPGEN) parser.skp parser_aux.c parser_gen.c
	$(SKPGEN) parser

$(SKPGEN): $(DEPSBIN)
	cd $(DEPSDIR); make

SYS_SRC=$(wildcard syslib/sys_*.c)
SYS_OBJ=$(SYS_SRC:.c=.o)

syslib.o: syslib.c syslib/syslib_proto.h 
	$(CC) $(CFLAGS) -c -o syslib.o syslib.c

syslib/syslib_proto.h: $(SYS_OBJ) syslib/build_sys
	cd syslib; cat sys_*.c | ./build_sys 2> syslib_proto.h | sort > syslib_tab.h 

syslib/build_sys: syslib/build_sys.o fname.o

clean:
	$(RM) *.o kajc parser.c parser.h
	$(RM) syslib/*.o syslib/syslib_proto.h syslib/syslib_tab.h syslib/build_sys


######## 

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




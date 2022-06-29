#  (C) by Remo Dentato (rdentato@gmail.com)
#  License: https://opensource.org/licenses/MIT

TOP = ../..
TESTS = $(subst t_,ut_,$(basename $(wildcard t_*.c) .c))

CC = gcc
CFLAGS =  -I $(TOP)/src -I$(TOP)/dist/src -O2 -Wall
DBGFLAGS = -DDEBUG=DBG_TEST

LIBS = -L $(TOP)/src -lkaj -lm 

.PRECIOUS: %.o

ls:
	@echo "Targets: all run runall clean $(TESTS)"

all: 
	@cd ../../src ; make
	@for f in $(TESTS) ; do \
     make $$f ;\
   done

run:
	@for f in $(TESTS) ; do \
	   echo -n "." ; \
     ./$$f 2>&1 | sed -n '/^FAIL:/p' ;\
   done
	@echo

runall: all run

%.o: %.c
	$(CC) $(CFLAGS) $(DBGFLGS) -c $*.c

u%: %.o
	$(CC) $(LNFLAGS) -o $@ $*.o $(LIBS)

clean:
	$(RM) *.o ut_* *.log



## #OPT+=-fdata-sections -ffunction-sections
## #OPT+=-ggdb3 -O0
## 
## #PROFFLAGS=-pg
## 
## LNFLAGS= $(PROFFLAGS) -L. -L %(TOP)/src -lkaj -lm 
## #LNFLAGS+=-s -Wl,--gc-sections
## 
## CFLAGS = $(PROFFLAGS) $(OPT) -Wall -I$(TOP)/src -I$(TOP)/dist/src
## 

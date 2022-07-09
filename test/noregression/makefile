# © 2022 Remo Dentato (rdentato@gmail.com)
# ⚖ MIT (https://opensource.org/licenses/MIT)

TOP = ../..
TESTS = $(subst t_,ut_,$(basename $(wildcard t_*.c) .c))

CC = gcc
CFLAGS =  -I $(TOP)/src -I$(TOP)/dist/src -O2 -Wall
DBGFLAGS = -DDEBUG=DBG_TEST

LIBS = -L $(TOP)/src -lkaj -lm 

.PRECIOUS: %.o 

ls:
	@echo "Targets: all run runall clean $(TESTS)$(subst ut_,x_,$(TESTS))"

all:
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

$(TOP)/src/libkaj.a: $(TOP)/src/*.c
	@cd $(TOP)/src ; make

%.o: %.c
	$(CC) $(CFLAGS) $(DBGFLGS) -c $*.c

u%: $(TOP)/src/libkaj.a %.o
	$(CC) $(LNFLAGS) -o $@ $*.o $(LIBS)

clean:
	$(RM) *.o ut_* *.log


# To test a single suite, for example STO, invoke:
#    make x_STO
# This will compile and execute the tests.

x_% : ut_%
	./ut_$* 2>&1

# The stderr file is redirected to stdout to ease further
# processing (for example with grep or sed)


# This is needed to avoid the removing of intermediate files
xALL: $(TESTS)  


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

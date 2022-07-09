# © 2021 Remo Dentato (rdentato@gmail.com)
# ⚖ MIT (https://opensource.org/licenses/MIT)

TOP=..

OPT=-O2
#OPT=-ggdb3 -O0

CFLAGS =  $(OPT)  -Wall -I$(TOP)/src -I$(TOP)/dist/src

.PRECIOUS: %.o
all:
	@ls ut_test*.c

test%: ut_test%.o
	$(CC) $(LNFLAGS) -o $@ ut_test$*.o 

clean:
	$(RM) *.o test*


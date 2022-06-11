TOP = ../..
SKP = $(TOP)/dist/bin/skpgen
INCDIR = $(TOP)/dist/src

CFLAGS =  -O2 -Wall -I $(INCDIR)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

all: skpgen kntest

kntest: knight_parse.o kntest.o
	$(CC) -s -o $@ knight_parse.o kntest.o

knight_parse.c: knight_parse.skp
	$(SKP) knight_parse

skpgen:
		@cd $(TOP)/dist; make

clean:
	$(RM) *.o knight_parse.c kntest *.t test/*.t
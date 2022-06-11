
TOP=../..

CC=gcc
#CC=clang

OPT=-O2
#OPT=-O2 -Os -fdata-sections -ffunction-sections

CFLAGS =  $(OPT) -Wall
#LNFLAGS=-Wl,--gc-sections

all: skpgen 

skpgen:
	cd src; $(CC) $(CFLAGS) -s -o ../bin/skpgen skpgen.c $(LNFLAGS)

clean:
	$(RM) src/*.o src/*.t bin/skpgen

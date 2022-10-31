
TOP=../..

CC=gcc
#CC=clang

OPT=-O2
#OPT=-O2 -Os -fdata-sections -ffunction-sections

CFLAGS =  $(OPT) -Wall
#LNFLAGS=-Wl,--gc-sections

binaries: bin/skpgen 

bin/skpgen: src/skpgen.c
	cd src; $(CC) $(CFLAGS) -s -o ../bin/skpgen skpgen.c $(LNFLAGS)

clean:
	$(RM) src/*.o src/*.t bin/skpgen

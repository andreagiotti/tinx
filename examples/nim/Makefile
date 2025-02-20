# Makefile

PROGS	= nim
HFILES	= nim.c ipc.c
HDEPS	= $(HFILES) tinx.h
HFLAGS	= -pipe -Ofast -Wall -lm -lrt -pthread `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
CC	= gcc

all:		$(PROGS)

nim:		$(HDEPS)
		$(CC) $(HFILES) -o nim $(HFLAGS)

clean:
		rm -f *.o *~ $(PROGS)


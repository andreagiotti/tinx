# Makefile

PROGS	= dynamic
HFILES	= dynamic.c ipc.c
HDEPS	= $(HFILES) tinx.h
HFLAGS	= -pipe -Ofast -Wall -lm -lrt -pthread `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
CC	= gcc

all:		$(PROGS)

dynamic:	$(HDEPS)
		$(CC) $(HFILES) -o dynamic $(HFLAGS)

clean:
		rm -f *.o *~ $(PROGS)


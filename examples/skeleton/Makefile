# Makefile

PROGS	= skeleton
HFILES	= skeleton.c ipc.c
HDEPS	= $(HFILES) tinx.h
HFLAGS	= -pipe -Ofast -Wall -lm -lrt -pthread `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
CC	= gcc

all:		$(PROGS)

skeleton:		$(HDEPS)
		$(CC) $(HFILES) -o skeleton $(HFLAGS)

clean:
		rm -f *.o *~ $(PROGS)


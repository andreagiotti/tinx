# Makefile

PROGS	= tinx tinx_mt ting tinxsh gtinxsh
XFILES	= tinx.c ipc.c
XDEPS	= $(XFILES) tinx.h
XFLAGS	= -pipe -Ofast -Wall -lrt
MFILES	= tinx_mt.c ipc_mt.c
MDEPS	= $(MFILES) tinx_mt.h
MFLAGS	= -pipe -Ofast -Wall -lrt -pthread
GFILES	= ting_lexer.c ting_parser.c ting_core.c
GDEPS	= $(GFILES) ting_lexer.h ting_parser.h ting_core.h
GFLAGS	= -pipe -Ofast -Wall
SFILES	= tinxsh.c ipc.c
SDEPS	= $(SFILES) tinxsh.h tinx_mt.h
SFLAGS	= -pipe -Ofast -Wall -lrt
UFILES	= gtinxsh.c ipc.c
UDEPS	= $(UFILES) gtinxsh.h tinx_mt.h ting_core.h
UFLAGS	= -pipe -Ofast -Wall -lrt -pthread `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
CC	= gcc

all:		$(PROGS)

ting_lexer.c:	ting.l ting_core.h tinx_mt.h
		flex ting.l

ting_parser.c:	ting.y ting_lexer.c ting_core.h tinx_mt.h
		bison ting.y

ting:		$(GDEPS)
		$(CC) $(GFILES) -o ting $(GFLAGS)

tinx:		$(XDEPS)
		$(CC) $(XFILES) -o tinx $(XFLAGS)

tinx_mt:	$(MDEPS)
		$(CC) $(MFILES) -o tinx_mt $(MFLAGS)

tinxsh:		$(SDEPS)
		$(CC) $(SFILES) -o tinxsh $(SFLAGS)

gtinxsh:	$(UDEPS)
		$(CC) $(UFILES) -o gtinxsh $(UFLAGS)

clean:
		rm -f *.o *~ $(PROGS)


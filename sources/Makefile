# Makefile

PROGS	= tinx tinx_mt tinx_dt tinx_zt ting gtinxsh extserv
XFILES	= tinx.c ipc.c
XDEPS	= $(XFILES) tinx.h
XFLAGS	= -pipe -Ofast -Wall -lm -lrt
MFILES	= tinx_mt.c ipc_mt.c
MDEPS	= $(MFILES) tinx_mt.h
MFLAGS	= -pipe -Ofast -Wall -lrt -pthread
BFILES	= tinx_dt.c ipc.c
BDEPS	= $(BFILES) tinx_dt.h
BFLAGS	= -pipe -Ofast -Wall -lm -lrt -pthread
ZFILES	= tinx_zt.c ipc_mt.c
ZDEPS	= $(ZFILES) tinx_zt.h
ZFLAGS	= -pipe -Ofast -Wall -lrt -pthread
GFILES	= ting_lexer.c ting_parser.c ting_core.c
GDEPS	= $(GFILES) ting_lexer.h ting_parser.h ting_core.h
GFLAGS	= -pipe -Ofast -Wall -lm
UFILES	= gtinxsh.c ipc.c
UDEPS	= $(UFILES) gtinxsh.h tinx_mt.h ting_core.h
UFLAGS	= -pipe -Ofast -Wall -lm -lrt -pthread `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
EFILES	= extserv.c ipc.c
EDEPS	= $(EFILES) tinx.h
EFLAGS	= -pipe -Ofast -Wall -lm -lrt `pkg-config gtk+-3.0 --cflags` `pkg-config gtk+-3.0 --libs`
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

tinx_dt:	$(BDEPS)
		$(CC) $(BFILES) -o tinx_dt $(BFLAGS)

tinx_zt:	$(ZDEPS)
		$(CC) $(ZFILES) -o tinx_zt $(ZFLAGS)

gtinxsh:	$(UDEPS)
		$(CC) $(UFILES) -o gtinxsh $(UFLAGS)

extserv:	$(EDEPS)
		$(CC) $(EFILES) -o extserv $(EFLAGS)

clean:
		rm -f *.o *~ $(PROGS)


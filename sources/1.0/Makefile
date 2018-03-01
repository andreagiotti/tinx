CC = gcc
CFLAGS = -pipe -Ofast -Wall -DNDEBUG
PROGS = tinx tint

all: $(PROGS)

tinx: tinx.c tinx.h
	$(CC) -o tinx tinx.c $(CFLAGS)

tint: tint.c tinx.h
	$(CC) -o tint tint.c $(CFLAGS)

clean:
	rm -f $(PROGS)


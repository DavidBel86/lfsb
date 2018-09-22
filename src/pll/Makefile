CC = gcc
CFLAGS = -O2 -s -std=c99 -Wall -pedantic -finline
CDFLAGS = -O2 -g -std=c99 -Wall -pedantic -finline -DDEBUG

all: pll

pll: *.c
	$(CC) $(CFLAGS) -c *.c

debug: CFLAGS=$(CDFLAGS)
debug:
	$(CC) $(CFLAGS) -c *.c

clean:
	rm -f *~ *.o

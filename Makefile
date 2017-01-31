
CC = gcc
CFLAGS = -c -Wall
VPATH = src

all: diskinfo disklist diskget diskput

diskinfo: diskinfo.o info.o
	$(CC) $^ -o $@

disklist: disklist.o info.o
	$(CC) $^ -o $@

diskget: diskget.o info.o
	$(CC) $^ -o $@

diskput: diskput.o info.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o diskinfo disklist diskget diskput

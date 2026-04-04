CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

all: server

server: server.c server.h ../lib/base32.c
	$(CC) $(CFLAGS) -o server server.c ../lib/base32.c

clean:
	rm -f server

.PHONY: all clean


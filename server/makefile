CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: server

server: server.c server.h base32/base32.c base32/base32.h
	$(CC) $(CFLAGS) -o server server.c base32/base32.c

clean:
	rm -f server

.PHONY: all clean


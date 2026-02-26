CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: server

server: server.c server.h
	$(CC) $(CFLAGS) -o server server.c

clean:
	rm -f server

.PHONY: all clean


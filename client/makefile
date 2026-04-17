CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99

all: client

client: client.c client.h ../lib/base32.c ../lib/dns_headers.h
	$(CC) $(CFLAGS) -o client client.c ../lib/base32.c

clean:
	rm -f client

.PHONY: all clean


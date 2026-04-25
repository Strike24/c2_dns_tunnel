CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 -Iinclude

# Source files
SHARED_SRCS = src/shared/base32.c src/shared/dns_protocol.c src/shared/networking.c
CLIENT_SRCS = src/client/client.c $(SHARED_SRCS)
SERVER_SRCS = src/server/server.c $(SHARED_SRCS)

# Object files
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

all: client server

client: bin/client

server: bin/server

bin/client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

bin/server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f bin/client bin/server $(CLIENT_OBJS) $(SERVER_OBJS)

.PHONY: all clean

#ifndef NETWORKING_H
#define NETWORKING_H

// Network
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#include "defs.h"

// Initialize socket and bind to port if type is SERVER
int init_socket(struct sockaddr_in *server_addr, program_type type);

#endif // NETWORKING_H
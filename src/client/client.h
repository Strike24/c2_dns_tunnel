#ifndef CLIENT_H
#define CLIENT_H
#include "dns_protocol.h"
#include "defs.h"
#include "networking.h"

int send_query(char *payload, int client_fd, struct sockaddr_in *server_addr, socklen_t server_addr_len);

#endif // CLIENT_H
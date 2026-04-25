#ifndef CLIENT_H
#define CLIENT_H
#include "dns_protocol.h"
#include "defs.h"
#include "networking.h"

// Constructs and sends a DNS query to the specified server with the given payload.
int send_query(const char *payload, int client_fd, struct sockaddr_in *server_addr, socklen_t server_addr_len);

#endif // CLIENT_H
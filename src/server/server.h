#ifndef SERVER_H
#define SERVER_H

#include "dns_protocol.h"
#include "defs.h"
#include "networking.h"

#include "lib/base32.h"

int send_response(const char *payload, int server_fd, char *query_buffer, struct sockaddr_in *client_addr, socklen_t client_addr_len, ssize_t received_bytes);
int parse_client_query(char *query_buffer, char *payload_buffer);
int parse_encoded_payload(const char *domain_name, char *payload_buffer, size_t buffer_size);

#endif // SERVER_H
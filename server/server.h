#ifndef SERVER_H
#define SERVER_H

#include "../lib/dns_headers.h"

// Function prototype for parsing domain names from DNS queries
int parse_domain_name(const char *buffer, char *domain_name, int max_len);

// Initializes socket and bind port
int init_socket(struct sockaddr_in *server_addr);
int format_answer(const char *payload, char *buffer);
int send_response(const char *payload, int server_fd, struct dns_header *recv_header, struct sockaddr_in *client_addr, socklen_t client_addr_len, ssize_t received_bytes);
#endif // SERVER_H
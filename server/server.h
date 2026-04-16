#ifndef SERVER_H
#define SERVER_H

// Network
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// Standard
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../lib/base32.h"

struct dns_header
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;    // Number of questions
    uint16_t ans_count;  // answer records
    uint16_t auth_count; // authority records
    uint16_t add_count;  // additional records
} __attribute__((packed));

struct dns_answer
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength; // Length of the RDATA field
    char rdata[256];   // field for the record data
} __attribute__((packed));

// Function prototype for parsing domain names from DNS queries
int parse_domain_name(const char *buffer, char *domain_name, int max_len);

// Initializes socket and bind port
int init_socket(struct sockaddr_in *server_addr);
int format_answer(const char *payload, char *buffer);
int send_response(const char *payload, int server_fd, struct dns_header *recv_header, struct sockaddr_in *client_addr, socklen_t client_addr_len, ssize_t received_bytes);
#endif // SERVER_H
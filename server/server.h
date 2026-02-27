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

struct dns_header
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;    // Number of questions
    uint16_t ans_count;  // answer records
    uint16_t auth_count; // authority records
    uint16_t add_count;  // additional records
} __attribute__((packed));

// Function prototype for parsing domain names from DNS queries
int parse_domain_name(const char *buffer, int offset, char *domain_name, int max_len);

#endif // SERVER_H
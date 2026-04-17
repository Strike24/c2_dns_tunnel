#ifndef DNS_HEADERS_H
#define DNS_HEADERS_H

// Network
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
// Standard
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Base32
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
    uint16_t cls;
    uint32_t ttl;
    uint16_t rdlength; // Length of the RDATA field
    char rdata[256];   // field for the record data
} __attribute__((packed));

#endif // DNS_HEADERS_H
#ifndef DNS_HEADERS_H
#define DNS_HEADERS_H

// Standard
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "networking.h"
#include "defs.h"

typedef struct
{
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;    // Number of questions
    uint16_t ans_count;  // answer records
    uint16_t auth_count; // authority records
    uint16_t add_count;  // additional records
} __attribute__((packed)) dns_header;

typedef struct
{
    char qname[MAX_DOMAIN_LEN];
    uint16_t qtype;
    uint16_t qclass;
} __attribute__((packed)) dns_question;

typedef struct
{
    uint16_t name;
    uint16_t type;
    uint16_t cls;
    uint32_t ttl;
    uint16_t rdlength; // Length of the RDATA field
    char rdata[];      // field for the record data
} __attribute__((packed)) dns_answer;

typedef enum
{
    A = 1,
    TXT = 16
} dns_type;

int format_answer_section(const char *payload, char *buffer, size_t buffer_size, dns_type ans_type);
int parse_qname(const char *buffer, char *domain_name);
#endif // DNS_HEADERS_H

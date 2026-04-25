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

/* Formats the answer section of a DNS response based on the provided payload and answer type.
 * @param payload The data to be included in the RDATA field of the DNS answer.
 * @param buffer The buffer where the formatted DNS answer will be stored.
 * @param buffer_size The size of the buffer to ensure it can accommodate the formatted answer.
 * @param ans_type The DNS record type (e.g., A, TXT) to be set in the answer section.
 * @return The total length of the formatted DNS answer on success, or an error code on failure.
 */
int format_answer_section(const char *payload, char *buffer, size_t buffer_size, dns_type ans_type);

/* Parses the QNAME (domain name) from a DNS query buffer and extracts the domain name, formatted as a standard dot-separated string.
 * @param qname The QNAME (domain name) to be parsed.
 * @param domain_name The output parameter where the extracted domain name will be stored.
 * @param buffer_size The size of the domain_name buffer to ensure it can accommodate the extracted domain name.
 * @return The number of bytes read on success, or an error code on failure.
 */
int parse_qname(const char *qname, char *buffer, size_t buffer_size);
#endif // DNS_HEADERS_H

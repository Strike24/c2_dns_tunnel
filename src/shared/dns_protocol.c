#include "dns_protocol.h"

int format_answer_section(const char *payload, char *buffer, size_t buffer_size, dns_type ans_type)
{
    size_t payload_len = strlen(payload);
    uint16_t rdata_length = payload_len + 1; // +1 for the length byte.
    uint16_t total_answer_len = sizeof(dns_answer) + rdata_length;
    dns_answer *answer = malloc(total_answer_len);
    if (answer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for DNS answer\n");
        return ERROR;
    }

    answer->name = htons(DNS_PTR_OFFSET); // Use the same domain from the question section
    answer->type = htons(ans_type);       // TODO - TXT FOR NOW, DYNAMICLY CHANGED BASED ON CONTEXT.
    answer->cls = htons(1);
    answer->ttl = htonl(300);
    answer->rdlength = htons(rdata_length);

    char *rdata_ptr = answer->rdata;

    if (payload_len > 255)
    {
        fprintf(stderr, "Payload too long for TXT record\n");
        free(answer);
        return ERROR;
    }

    rdata_ptr[0] = (char)payload_len;            // Length byte
    memcpy(rdata_ptr + 1, payload, payload_len); // Copy the payload after the length byte

    if (buffer_size < total_answer_len)
    {
        fprintf(stderr, "Buffer size too small for DNS answer\n");
        free(answer);
        return ERROR;
    }
    memcpy(buffer, answer, total_answer_len);

    free(answer);
    return total_answer_len;
}

int parse_qname(const char *buffer, char *domain_name)
{
    int offset = sizeof(dns_header);
    unsigned char *start = (unsigned char *)buffer + offset;
    unsigned char *reader = start;
    int section_len = *reader;
    int qname_len = 0;

    while (section_len > 0)
    {
        reader++;

        if ((long unsigned int)(qname_len + section_len) >= MAX_DOMAIN_LEN)
        {
            fprintf(stderr, "Domain name too long\n");
            return ERROR;
        }
        strncat(domain_name, (char *)reader, section_len);
        qname_len += section_len;
        reader += section_len;
        if (*reader != 0)
        {
            domain_name[qname_len] = '.';
            domain_name[qname_len + 1] = '\0';
            qname_len++;
        }
        section_len = *reader;
    }
    int bytes_read = (reader - start) + 1; // +1 for the null byte
    return bytes_read;
}

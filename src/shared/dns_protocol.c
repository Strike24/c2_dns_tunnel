#include "dns_protocol.h"

int format_answer_section(const char *payload, char *buffer, size_t buffer_size, dns_type ans_type)
{
    size_t payload_len = strlen(payload);
    uint16_t rdata_length = payload_len + 1; // +1 for the length byte.
    uint16_t total_answer_len = sizeof(dns_answer) + rdata_length;
    dns_answer *answer = malloc(total_answer_len);
    memset(answer, 0, total_answer_len); // Zero out the allocated memory
    if (answer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for DNS answer\n");
        return ERROR;
    }

    answer->name = htons(DNS_PTR_OFFSET); // Use the same domain from the question section
    answer->type = htons(ans_type);
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

char *encode_qname(const char *domain_name)
{
    char buffer[MAX_DOMAIN_LEN + 2] = {0}; // +2 for the length byte and null terminator
    size_t domain_len = strlen(domain_name);
    if (domain_len + 2 > sizeof(buffer))
    {
        fprintf(stderr, "Buffer size too small for encoded QNAME\n");
        return NULL;
    }

    const char *label_start = domain_name;
    char *buffer_ptr = buffer;

    // Loop through each label, add length byte before the label, and copy the label to the buffer
    while (*label_start)
    {
        const char *label_end = strchr(label_start, '.');
        if (!label_end)
        {
            label_end = domain_name + domain_len; // Point to the end of the string
        }

        size_t label_len = label_end - label_start;
        if (label_len > MAX_LABEL_LEN)
        {
            fprintf(stderr, "Label length exceeds maximum allowed length\n");
            return NULL;
        }

        *buffer_ptr++ = (char)label_len;            // Write the length byte
        memcpy(buffer_ptr, label_start, label_len); // Copy the label
        buffer_ptr += label_len;

        if (*label_end == '.')
        {
            label_start = label_end + 1; // Move to the start of the next label
        }
        else
        {
            break; // No more labels
        }
    }
    *buffer_ptr++ = 0;                     // Null byte to terminate the QNAME
    int encoded_len = buffer_ptr - buffer; // Total length of the encoded QNAME

    char *encode_qname = calloc(1, encoded_len);
    if (encode_qname == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for encoded QNAME\n");
        return NULL;
    }
    memcpy(encode_qname, buffer, encoded_len);
    return encode_qname;
}

int parse_qname(const char *qname, char *buffer, size_t buffer_size)
{
    int offset = sizeof(dns_header);
    unsigned char *start = (unsigned char *)qname + offset;
    unsigned char *reader = start;
    int section_len = *reader;
    int qname_len = 0;

    while (section_len > 0)
    {
        reader++;

        if ((long unsigned int)(qname_len + section_len) >= buffer_size)
        {
            fprintf(stderr, "Buffer too small for domain name\n");
            return ERROR;
        }
        strncat(buffer, (char *)reader, section_len);
        qname_len += section_len;
        reader += section_len;
        if (*reader != 0)
        {
            buffer[qname_len] = '.';
            buffer[qname_len + 1] = '\0';
            qname_len++;
        }
        section_len = *reader;
    }
    int bytes_read = (reader - start) + 1; // +1 for the null byte
    return bytes_read;
}

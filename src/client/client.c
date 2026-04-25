#include "client.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [server IP]\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr =
        {
            .sin_family = AF_INET, // IPV4
            .sin_port = htons(PORT),
            .sin_addr = {0},
            .sin_zero = {0}};

    if (!argv[1] || inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "Invalid IP Adress in the first argument.");
        return EXIT_FAILURE;
    }

    int client_fd = init_socket(&server_addr, CLIENT);
    if (client_fd == -1)
    {
        fprintf(stderr, "Socket initializing failed, exiting program.");
        return EXIT_FAILURE;
    }

    int res_length = send_query("obuw4zy.example.com", client_fd, &server_addr, sizeof(server_addr), A);
    if (res_length < 0)
    {
        fprintf(stderr, "Failed to send query to the server, exiting.");
        return EXIT_FAILURE;
    }
}

int send_query(const char *payload, int client_fd, struct sockaddr_in *server_addr, socklen_t server_addr_len, dns_type qtype)
{
    char packet[512] = {0};
    int packet_offset = 0;

    // Create header & copy to packet
    dns_header query_header = {0};
    query_header.id = htons(rand() % 65536); // Random ID
    query_header.flags = htons(0x0100);      // Standard query
    query_header.qdcount = htons(1);         // One question

    // Copy the header to the packet buffer
    memcpy(packet, &query_header, sizeof(query_header));

    packet_offset += sizeof(dns_header);

    // Question section
    dns_question_attr question = {0};
    char *qname_ptr = encode_qname(payload);
    if (qname_ptr == NULL)
    {
        fprintf(stderr, "Failed to encode QNAME");
        return ERROR;
    }

    question.qtype = htons(qtype);
    question.qclass = htons(1);

    // Copy the QNAME to the packet buffer
    memcpy(packet + packet_offset, qname_ptr, strlen(qname_ptr) + 1); // +1 for the null byte
    packet_offset += strlen(qname_ptr) + 1;
    // Copy the question attributes to the packet buffer
    memcpy(packet + packet_offset, &question, sizeof(question));
    packet_offset += sizeof(question);

    ssize_t sent_len = sendto(client_fd, packet, packet_offset, 0,
                              (const struct sockaddr *)server_addr, server_addr_len);
    if (sent_len == -1)
    {
        perror("sendto failed");
        free(qname_ptr);
        return ERROR;
    }
    free(qname_ptr);
    return 0;
}

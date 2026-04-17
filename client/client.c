#include "client.h"

int init_socket()
{
    // Create socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1)
    {
        perror("socket creation failed");
        return -1;
    }
    return server_fd;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [server IP]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET; // IPv4
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    server_addr.sin_port = htons(8053);

    int client_fd = init_socket();
    if (client_fd == -1)
    {
        exit(EXIT_FAILURE);
    }
}

int send_query(char *payload, int client_fd, struct sockaddr_in *server_addr, socklen_t server_addr_len)
{
    char packet[512] = {0};
    int packet_offset = 0;

    // --- Create header & copy to packet ---
    struct dns_header query_header = {0};
    query_header.id = htons(rand() % 65536); // Random ID
    query_header.flags = htons(0x0100);      // Standard query
    query_header.qdcount = htons(1);         // One question

    // Copy the header to the packet buffer
    void *result = memcpy(packet, &query_header, sizeof(query_header));
    if (result == NULL)
    {
        fprintf(stderr, "Failed to copy DNS header to packet buffer\n");
        return -1;
    }
    packet_offset += sizeof(query_header);

    // // --- Create question and copy to packet ---
    // char domain_name[256] = {0};
    // int domain_name_len = create_dns_domainname(payload, domain_name, sizeof(domain_name));
    // if (domain_name_len < 1)
    // {
    //     fprintf(stderr, "Failed to create domain name for dns query\n");
    //     return -1;
    // }

    ssize_t sent_len = sendto(client_fd, packet, packet_offset, 0,
                              (const struct sockaddr *)server_addr, server_addr_len);
    if (sent_len == -1)
    {
        perror("sendto failed");
        return -1;
    }
    return 0;
}

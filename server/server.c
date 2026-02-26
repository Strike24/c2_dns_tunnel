#include "server.h"
#define PORT 8053

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(PORT);

    // Create socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to the address
    int bind_result = bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bind_result == -1)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Server loop, waiting for incoming messages
    while (1)
    {
        char response_buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        ssize_t recv_len = recvfrom(server_fd, response_buffer, sizeof(response_buffer) - 1, 0,
                                    (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_len > 0)
        {
            response_buffer[recv_len] = '\0'; // Null-terminate the received data
            printf("Received message from client:");

            for (ssize_t i = 0; i < recv_len; i++)
            {
                printf(" %02x", (unsigned char)response_buffer[i]);
            }
            printf("\n");

            struct dns_header *header = (struct dns_header *)response_buffer;
            printf("DNS Header:\n");
            printf("  ID: %u\n", ntohs(header->id));
            printf("  Flags: %u\n", ntohs(header->flags));
            printf("  Questions: %u\n", ntohs(header->qdcount));
            printf("  Answer RRs: %u\n", ntohs(header->ans_count));
            printf("  Authority RRs: %u\n", ntohs(header->auth_count));
            printf("  Additional RRs: %u\n", ntohs(header->add_count));
                }
        else if (recv_len == -1)
        {
            perror("recvfrom failed");
        }
    }
}
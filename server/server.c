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

int main(int argc, char *argv[])
{
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

            printf("Received message from client.");

            // // Print the raw bytes in hex
            // for (ssize_t i = 0; i < recv_len; i++)
            // {
            //     printf(" %02x", (unsigned char)response_buffer[i]);
            // }
            // printf("\n");

            char domain_name[256] = {0};
            domain_name[0] = '\0';
            int bytes_read = parse_domain_name(response_buffer, domain_name, sizeof(domain_name));
            if (bytes_read > 0)
            {
                printf("Parsed domain name: %s\n", domain_name);
            }
            else
            {
                perror("Failed to parse domain name");
            }
            send_response(server_fd, header, &client_addr, client_addr_len);
        }
        else if (recv_len == -1)
        {
            perror("recvfrom failed");
        }
    }
}

int send_response(int server_fd, struct dns_header *header, struct sockaddr_in *client_addr, socklen_t client_addr_len)
{

    struct dns_header response_header = {0};
    response_header.id = header->id;
    response_header.flags = htons(0x8180);     // Standard query response, No error
    response_header.qdcount = header->qdcount; // Echo back the question count
    response_header.ans_count = htons(1);      // One answer record

    // Answer Record
    char answer_buffer[512] = {0};

    ssize_t sent_len = sendto(server_fd, &response_header, sizeof(response_header), 0,
                              (const struct sockaddr *)client_addr, client_addr_len);
    if (sent_len == -1)
    {
        perror("sendto failed");
        return -1;
    }
    return 0;
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
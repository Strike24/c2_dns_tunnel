#include "networking.h"
#include "defs.h"

int init_socket(struct sockaddr_in *server_addr, program_type type)
{
    // Create socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("socket creation failed");
        return -1;
    }

    // Bind socket to the address
    if (type == SERVER)
    {
        int bind_result = bind(fd, (const struct sockaddr *)server_addr, sizeof(*server_addr));
        if (bind_result == -1)
        {
            perror("bind failed");
            if (fd)
                close(fd);
            return ERROR;
        }
    }

    return fd;
}
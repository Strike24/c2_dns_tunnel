#ifndef DEFS_H
#define DEFS_H

#define PORT 8053
#define DNS_FLAG_OK 0x8180
#define DNS_PTR_OFFSET 0xC00C
#define DNS_TYPE_TXT 16
#define DNS_TYPE_A 1
#define MAX_DOMAIN_LEN 255

#define ERROR -1

typedef enum
{
    SERVER,
    CLIENT
} program_type;

#endif // DEFS_H
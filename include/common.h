#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>

#define PORT 5555
#define BUF_SIZE 4096
#define MAX_CLIENT 10

typedef enum {
    PROTO_HELLO,
} proto_type_e;

// TLV
typedef struct {
    proto_type_e type;
    unsigned short len;
} proto_hdr_t;

typedef enum {
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
} state_e;

typedef struct {
    int fd;
    state_e state;
    char buffer[BUF_SIZE];
} clientstate_t;

extern clientstate_t clientStates[MAX_CLIENT]; // extern declaration

extern struct sockaddr_in server_addr; // extern declaration
extern struct sockaddr_in client_addr; // extern declaration

#endif

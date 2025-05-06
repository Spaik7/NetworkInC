#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/Server_handling.h"
#include "../include/common.h"

void handle_client(int fd)
{
    char buff[BUF_SIZE] = {0};
    proto_hdr_t *hdr = (proto_hdr_t *)buff;
    hdr->type=htonl(PROTO_HELLO);

    hdr->len=sizeof(int);
    int reallen = hdr->len;

    hdr->len = htons(hdr->len);

    int *data = (int*)&hdr[1];
    *data = htonl(1);

    write(fd, hdr, sizeof(proto_hdr_t)+ reallen);
}

void init_server()
{
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
}

void init_clients()
{
    for(int i=0;i<MAX_CLIENT;i++)
    {
        clientStates[i].fd = -1; // free slot
        clientStates[i].state = STATE_NEW;
        memset(&clientStates[i].buffer, '\0', BUF_SIZE);
    }
}

int find_free_slot()
{
    for(int i=0; i<MAX_CLIENT; i++)
    {
        if(clientStates[i].fd==-1)
            return i;
    }
    return -1;//no slot
}

int find_slot_by_fd(int fd)
{
    for(int i=0;i <MAX_CLIENT; i++)
    {
        if(clientStates[i].fd==fd)
            return i;
    }
    return -1;//not found
}
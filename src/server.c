#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <stdlib.h>

#include "../include/Server_handling.h"
#include "../include/common.h"

clientstate_t clientStates[MAX_CLIENT]; // actual definition
struct sockaddr_in server_addr = {0};   // actual definition
struct sockaddr_in client_addr = {0};

int main()
{
    int listen_fd, conn_fd, freeSlot;
    socklen_t client_len = sizeof(client_addr);

    struct pollfd fds[MAX_CLIENT+1];
    int nfds =1;
    int opt = 1;

    init_clients();

    
    if((listen_fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        perror("socket");
        exit(-1);
    }

    if(setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(-1);
    }
    
    init_server();

    if(bind(listen_fd, (struct sockaddr *)&server_addr,sizeof(server_addr))==-1)
    {
        perror("bind");
        exit(-1);
    }

    if(listen(listen_fd, 10)==-1)
    {
        perror("listen");
        exit(-1);
    }

    printf("Server listening on port %d\n", PORT);

    memset(fds, 0, sizeof(fds));
    fds[0].fd=listen_fd;
    fds[0].events = POLLIN;
    nfds = 1;

    while(1)
    {
        int ii = 1;
        for(int i=0;i < MAX_CLIENT;i++)
        {
            if(clientStates[i].fd !=-1)
            {
                fds[ii].fd=clientStates[i].fd;
                fds[ii].events=POLLIN;
                ii++;
            }
        }

        int n_events = poll(fds, nfds, -1);
        if(n_events == -1)
        {
            perror("poll");
            exit(-1);
        }

        if(fds[0].revents & POLLIN)
        {
            if((conn_fd=accept(listen_fd, (struct sockaddr * )&client_addr,&client_len))==-1)
            {
                perror("accept");
                continue;
            }

            printf("New connection from %s: %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            freeSlot=find_free_slot();
            if(freeSlot==-1)
            {
                printf("Server full: closing new connection");
                close(conn_fd);
            }
            else 
            {
                clientStates[freeSlot].fd=conn_fd;
                clientStates[freeSlot].state=STATE_CONNECTED;
                nfds++;
                printf("Slot %d has fd %d\n", freeSlot, clientStates[freeSlot].fd);
            }

            n_events--;

        }

        for(int i=1;i<=nfds&&n_events>0;i++)
        {
            if(fds[i].revents & POLLIN)
            {
                n_events--;

                int fd = fds[i].fd;
                int slot = find_free_slot();
                ssize_t bytes_read = read(fd, &clientStates[slot].buffer, sizeof(clientStates[slot].buffer));
                if(bytes_read<=0)
                {
                    close(fd);
                    if(slot==-1)
                    {
                        printf("Tried to close fd that doesn't exist?\n");
                    }
                    else 
                    {
                        clientStates[slot].fd = -1;
                        clientStates[slot].state=STATE_DISCONNECTED;
                        printf("Client disconnected or error\n");
                        nfds--;
                    }
                }
                else
                {
                    printf("Recived data from client: %s\n",clientStates[slot].buffer);
                }

            }
        }
        return 0;
    }


}
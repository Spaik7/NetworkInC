#ifndef SERVER_H
#define SERVER_H

void init_server();
void handle_client(int fd);
void init_clients();
int find_free_slot();
int find_slot_by_fd(int fd);
#endif
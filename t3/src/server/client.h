#ifndef CLIENT_H
#define CLIENT_H
#include <stdint.h>
#include<pthread.h>

/* maximum length of username */
#define USER_NICK_MAX_LENGTH 20

typedef struct client {
    int client_id;
    char nick[USER_NICK_MAX_LENGTH + 1];
    char nick_is_set;
    int socket;
    pthread_mutex_t socket_write_lock;
    int room_id; /*-1 if not in any room*/
    struct client *client_next;
} CLIENT;

void client_connect(int client_id, int socket);
int send_message_to_client(CLIENT *client, int type, int length, int sequence_number, char *data);
#endif

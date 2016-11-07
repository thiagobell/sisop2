#ifndef CLIENT_H
#define CLIENT_H
#include <stdint.h>
#include<pthread.h>

/* This module implements operations dealing with the
 * representation of each user on the server. Each user
 * is represented as a thread.
 *
 * when a user enters a room, a reference their CLIENT
 * struct is stored inside the rooms data structure. Any
 * modification will be automatically reflected. The ROOM
 * module SHOULD NOT modify the client data structure except
 * for the client_next field.
*/

/* maximum length of username */
#define USER_NICK_MAX_LENGTH 20



typedef struct client {
    int client_id;
    char nick[USER_NICK_MAX_LENGTH + 1];
    char nick_is_set;
    int socket;
    /*write lock is important because other threads may want to write.
     * this socket SHOULD NOT be used from outside this module except
     * through the send_message_to_client function */
    pthread_mutex_t socket_write_lock;
    int room_id; /*-1 if not in any room*/
    struct client *client_next;
    pthread_t thread;
} CLIENT;

void* client_connect(void *client_ds) ;
int send_message_to_client(CLIENT *client, int type, int length, int sequence_number, char *data);
#endif

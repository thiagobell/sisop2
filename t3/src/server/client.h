#ifndef CLIENT_H
#define CLIENT_H
#include <stdint.h>

/* maximum length of username */
#define USER_NICK_MAX_LENGTH 20

typedef struct client {
    int client_id;
    char nick[USER_NICK_MAX_LENGTH + 1];
    int socket;
    struct client *client_next;
} CLIENT;
#endif

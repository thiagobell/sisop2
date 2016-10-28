#ifndef ROOM_H
#define ROOM_H
#include <stdint.h>
#include "client.h"
#include<pthread.h>

/* maximum length of a room's name*/
#define ROOM_NAME_MAX_LENGTH 20
/* maximum number of rooms*/
#define ROOM_MAX_NUM 20
/* maximum number of clients per room*/
#define ROOM_MAX_CLIENTS 40

typedef struct {
    char room_name[ROOM_NAME_MAX_LENGTH + 1];
    /* current number of connected clients */
    uint32_t client_num;
    CLIENT *client_list;
    pthread_mutex_t room_lock;
}ROOM;

/* must be called before any operation*/
void initialize();

//returns -1 if room not found
int get_room_id_by_name(char *name);

/* creates a room and returns its id
if room already exists, returns id*/
int create_room(char *name);

/* adds client to room
 * returns 0 if successful
 * returns -1 if room does not exist
 * returns -2 if room is full
 if client is already in room returns successful
*/
int add_client_to_room(CLIENT client, char *room_name);

/* removes client from room
    returns 0 if removed succesfully
    returns -1 if rooms is empty
*/
int remove_client_from_room(int client_id, char *room_name);

int get_num_clients_in_room(char *name);

void print_clients_in_room(char *name);

#endif


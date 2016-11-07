#ifndef ROOM_H
#define ROOM_H
#include <stdint.h>
#include "client.h"
#include<pthread.h>

/* This module defines operations over the room data structure
 * It stores information about every existing room on the server
 * as well as references to the clients connected to it. Sychronization
 * of threads is done using mutexes.
*/

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
if room already exists, returns id
if max num of rooms was reached return -1*/
int create_room(char *name);

/* adds client to room
 * returns 0 if successful or client is already in room
 * returns -1 if room does not exist
 * returns -2 if room is full
 * returns -3 if client is in another room
*/
int add_client_to_room(CLIENT *client, char *room_name);

/* removes client from room
    returns 0 if removed succesfully
    returns -1 if rooms is empty
    returns -2 if client is not in room
*/
int remove_client_from_room(CLIENT *client);

int get_num_clients_in_room(char *name);

void print_clients_in_room(char *name);

char* get_room_names();

/*sends message to every client in the room except the sender
returns -1 if room does not exist. 0 if successful*/
int send_message_to_room(int room_id, CLIENT *cli, char *message);

#endif


#ifndef ROOM_H
#define ROOM_H
#include <stdint.h>

/* maximum length of a room's name*/
#define ROOM_NAME_MAX_LENGTH
/* maximum number of rooms*/
#define ROOM_MAX_NUM
/* maximum number of clients per room*/
#define ROOM_MAX_CLIENTS

typedef struct {
    char room_name[ROOM_NAME_MAX_SIZE + 1];
    uint32_t room_id;
    /* current number of connected clients */
    uint32_t client_num;

}ROOM;

uint32_t get_room_id_by_name(char *name);

/* creates a room and returns its id*/
uint32_t create_room(char *name);

/* adds client to room */
uint32_t add_client_to_room(uint32_t client);

/* removes client from room */
uint32_t add_client_to_room(uint32_t client);


#endif


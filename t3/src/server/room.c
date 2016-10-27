#include "room.h"
#include <stdint.h>

/*maps which position on array rooms is available*/
uint8_int roommap[ROOM_MAX_NUM];
/* stores rooms data structures */
ROOM rooms[ROOM_MAX_NUM];

char initialized = 0;

void initialize() {
    int i;
    for(i = 0; i < ROOM_MAX_NUM; i++) {
        roommap[i] = 0;
    }
    initialized = 1;
}

uint32_t get_room_id_by_name(char *name) {
    printf("function not implemented\n");
    exit(-1);
}


uint32_t create_room(char *name) {
    printf("function not implemented\n");
    exit(-1);
}

uint32_t add_client_to_room(uint32_t client){
    printf("function not implemented\n");
    exit(-1);
}


uint32_t add_client_to_room(uint32_t client) {
    printf("function not implemented\n");
    exit(-1);
}

#include "room.h"

#include <stdint.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/message.h"


/*maps which position on array rooms is available*/
char roommap[ROOM_MAX_NUM];
/* stores rooms data structures */
ROOM rooms[ROOM_MAX_NUM];
/*counts the number of rooms*/


/* mutex for manipulating the room  list datastructure */
pthread_mutex_t room_list_lock;
pthread_mutexattr_t Attr;

int room_num;
char initialized = 0;

void initialize() {
    int i;
    for(i = 0; i < ROOM_MAX_NUM; i++) {
        roommap[i] = 0;
    }
    initialized = 1;
    room_num = 0;

    //initializes a recursive mutex
    pthread_mutexattr_init(&Attr);
    pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);

    if (pthread_mutex_init(&room_list_lock, &Attr) != 0)
    {
        printf("ERROR: Init of mutex for room list failed\n");
        exit(-1);
    }



}

void check_if_inited(){
    if(initialized == 0) {
        printf("ERROR: Room datastructure used before initialization\n");
        exit(-1);
    }
}

int get_room_id_by_name(char *name) {
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    int room_index;
    for(room_index = 0; room_index<ROOM_MAX_NUM; room_index++) {
        if(roommap[room_index] != 0) {
            if(strcmp(name,rooms[room_index].room_name) == 0) {
                //found room
                pthread_mutex_unlock(&room_list_lock);
                return room_index;
            }
        }
    }
    pthread_mutex_unlock(&room_list_lock);
    return -1;
}


int create_room(char *name) {
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    //tests if rooms does not already exists
    int test = get_room_id_by_name(name);
    if(test >= 0) {
        //rooms exists
        return test;
    }
    if(room_num == ROOM_MAX_NUM){
        //max number of rooms reached
        return -1;
    }
    room_num++;
    int room_index;
    for(room_index = 0; room_index<ROOM_MAX_NUM; room_index++) {
        if(roommap[room_index] == 0) {
            if (pthread_mutex_init(&rooms[room_index].room_lock, NULL) != 0)
            {
                printf("ERROR: Init of mutex for room failed\n");
                exit(-1);
            }
            pthread_mutex_lock(&rooms[room_index].room_lock);
            roommap[room_index] = 1;
            strncpy(rooms[room_index].room_name, name, sizeof(rooms[room_index].room_name));
            rooms[room_index].client_num = 0;
            rooms[room_index].client_list = NULL;
            pthread_mutex_unlock(&rooms[room_index].room_lock);
            break;
        }
    }
    pthread_mutex_unlock(&room_list_lock);
    return room_index;
}

int add_client_to_room(CLIENT *client, char *name){
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    //tests if rooms does not already exists
    int test = get_room_id_by_name(name);
    if(test < 0) {
        //rooms does not exists
        pthread_mutex_unlock(&room_list_lock);
        return -1;
    }
    pthread_mutex_lock(&rooms[test].room_lock);
    pthread_mutex_unlock(&room_list_lock);


    if( rooms[test].client_num == ROOM_MAX_CLIENTS) {
            //room is full
            pthread_mutex_unlock(&rooms[test].room_lock);
            return -2;
    }




    if(rooms[test].client_list == NULL) {
        //list is empty. put at top.
        rooms[test].client_num++;
        rooms[test].client_list = client;
        client->room_id = test;
    } else {
        CLIENT *current = rooms[test].client_list;

        while(current->client_next != NULL) {
            if(current->client_id == client->client_id) {
                //client is already in room
                pthread_mutex_unlock(&rooms[test].room_lock);
                return 0;

            }
            current = current->client_next;

        }
         //must test if this node has same client id
        if(current->client_id == client->client_id) {
            //client is already in room
            pthread_mutex_unlock(&rooms[test].room_lock);
            return 0;
        }
        rooms[test].client_num++;
        current->client_next = client;
        client->room_id = test;

    }

    pthread_mutex_unlock(&rooms[test].room_lock);
    return 0;

}


int remove_client_from_room(int client_id, char *name) {
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    //tests if rooms does not already exists
    int test = get_room_id_by_name(name);
    if(test < 0) {
        //rooms does not exists
        pthread_mutex_unlock(&room_list_lock);
        return -1;
    }
    pthread_mutex_lock(&rooms[test].room_lock);
    pthread_mutex_unlock(&room_list_lock);
    if(rooms[test].client_num == 0) {
        //room is empty
        pthread_mutex_unlock(&rooms[test].room_lock);
        return -1;
    } else {
        CLIENT *current = rooms[test].client_list;
        //tests first element
        if(current->client_id == client_id) {
            CLIENT *temp = rooms[test].client_list->client_next;
            rooms[test].client_list = temp;
            free(current);
            rooms[test].client_num--;
        } else {
            //tests other elements
            while(current->client_next != NULL) {
                if(current->client_next->client_id == client_id){
                    CLIENT *temp = current->client_next->client_next;
                    current->client_next->room_id = -1;
                    current->client_next = temp;
                    rooms[test].client_num--;
                    break;
                }
            }
        }
        pthread_mutex_unlock(&rooms[test].room_lock);
        return 0;

    }
}

int get_num_clients_in_room(char *name)
{
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    //tests if rooms does not already exists
    int test = get_room_id_by_name(name);
    if(test < 0) {
        //rooms does not exists
        pthread_mutex_unlock(&room_list_lock);
        return -1;
    }
    int num = rooms[test].client_num;
    pthread_mutex_unlock(&room_list_lock);
    return num;
}

void print_clients_in_room(char *name)
{
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    //tests if rooms does not already exists
    int test = get_room_id_by_name(name);
    if(test < 0) {
        //rooms does not exists
        pthread_mutex_unlock(&room_list_lock);
        return;
    }
    pthread_mutex_lock(&rooms[test].room_lock);
    pthread_mutex_unlock(&room_list_lock);
    printf("client ids on room %s:", name);
    int client;
    CLIENT *cl;
    cl = rooms[test].client_list;
    for(client=0;client< rooms[test].client_num; client++) {
        printf(" %d", cl->client_id);
        cl = cl->client_next;
        if(cl == NULL)
            break;
    }

    pthread_mutex_unlock(&rooms[test].room_lock);
    printf("done\n");
}

char *get_room_names()
{
    int room_index;
    char *names;
    names = (char*) calloc(sizeof(char), ROOM_MAX_NUM*ROOM_NAME_MAX_LENGTH +1);
    int bytes_written = 0;
    check_if_inited();
    pthread_mutex_lock(&room_list_lock);
    for(room_index=0;room_index<ROOM_MAX_NUM;room_index++) {
        if(roommap[room_index] == 1) {
            int len = strlen(rooms[room_index].room_name);
            strncpy(names+bytes_written, rooms[room_index].room_name, len);
            bytes_written += len;
            strncpy(names+bytes_written, "\n",1 );
            bytes_written += 1;
        }
    }
    pthread_mutex_unlock(&room_list_lock);
    if(bytes_written > 0) {
        strncpy(names+bytes_written-1, "\0",1 );
    } else {
        strncpy(names, "\0",1 );
        bytes_written++;
    }


    char *names_final = (char*) calloc(sizeof(char), bytes_written);
    memcpy(names_final, names, bytes_written);
    free(names);

    return names_final;
}

//-1 room does not exist
int send_message_to_room(int room_id, CLIENT *cli, char *message)
{
    check_if_inited();
    CLIENT *clients[ROOM_MAX_CLIENTS];
    int num_clients;
    pthread_mutex_lock(&room_list_lock);
    //tests if rooms does not already exists

    if(roommap[room_id]==0) {
        //rooms does not exists
        pthread_mutex_unlock(&room_list_lock);
        return -1;
    }
    pthread_mutex_lock(&rooms[room_id].room_lock);
    pthread_mutex_unlock(&room_list_lock);
    int client_id;
    CLIENT *cl;
    cl = rooms[room_id].client_list;
    for(client_id=0;client_id< rooms[room_id].client_num; client_id++) {
        num_clients++;
        clients[client_id] = cl;
        cl = cl->client_next;
        if(cl == NULL)
            break;
    }
    pthread_mutex_unlock(&rooms[room_id].room_lock);
    int cursor;
    for(cursor = 0; cursor < num_clients; cursor++) {
        if(clients[cursor]->room_id == room_id)
            send_message_to_client(clients[cursor], MESSAGE_CHAT_MSG_TO_CLIENT,strlen(message)+1, 0, message);

    }
    printf("done\n");
}

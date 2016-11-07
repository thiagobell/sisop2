#include "client.h"
#include "../shared/message.h"
#include "room.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int client_id = 0;

void set_nick(CLIENT *client, MESSAGE *msg) {
        if(msg->length < 0) {
            //invalid nick
            send_message_to_client(client, MESSAGE_ERROR_INVALID_NICK,0, msg->sequence_number, NULL);
        } else {
            strncpy(client->nick, msg->data,USER_NICK_MAX_LENGTH +1);
            printf("new nick is %s\n", msg->data);
            client->nick_is_set = 1;
            send_message_to_client(client, MESSAGE_OK,0, msg->sequence_number, NULL);
        }
}

void disconnect_cleanly(CLIENT *client) {

 close(client->socket);
 remove_client_from_room(client);
 free(client);
 printf("client disconnected and removed from server\n");
 pthread_exit(0);
}

//handles message of type MESSAGE_CHAT_MSG
void handle_chat_message(CLIENT *client, MESSAGE *msg) {

    if(client->room_id == -1) {
        //client not in any room
        printf("client %d not in room\n", client->client_id);
        return;
    }
    int len_message = strlen(msg->data);
    int len_nick = strlen(client->nick);
    char *full_message = calloc(1, len_message+len_nick+3);
    strncpy(full_message, client->nick, len_nick);
    full_message[len_nick] = ':';

    strncpy(full_message+len_nick+1, msg->data, len_message);
    printf("about to send message from client %d\n", client->client_id);
    send_message_to_room(client->room_id,client, full_message);
   // printf("message received from clientid %d on roomid %d. %s", client->client_id,client->room_id,msg->data);
    printf("Forward message:%s", full_message);
    free(full_message);
}
void handle_enter_room(CLIENT *client, MESSAGE *msg) {

    if(!(client->nick_is_set)) {
        send_message_to_client(client, MESSAGE_ERROR_NICK_NOT_SET,0,msg->sequence_number,NULL);
        printf("nick is not set\n");
        return;
    }
    if(client->room_id!=-1) {
        send_message_to_client(client, MESSAGE_ERROR_ALREADY_IN_A_ROOM, 0,msg->sequence_number,NULL);
        return;
    }
    int result = create_room(msg->data);
    if(result == -1) {
        send_message_to_client(client, MESSAGE_ERROR_MAX_ROOM_REACHED, 0,msg->sequence_number,NULL);
        return;
    }
    result = add_client_to_room(client, msg->data);
    if(result == -2) {
        //room is full
        send_message_to_client(client, MESSAGE_ERROR_ROOM_IS_FULL, 0,msg->sequence_number,NULL);
        return;
    }
    send_message_to_client(client, MESSAGE_OK, 0,msg->sequence_number,NULL);
    print_clients_in_room(msg->data);
}
void handle_exit_room(CLIENT *client, MESSAGE *msg) {
    if(client->room_id == -1) {
        //client not in room
        send_message_to_client(client, MESSAGE_ERROR_NOT_IN_ROOM, 0,msg->sequence_number,NULL);
        return;
    }
    remove_client_from_room(client);
    client->room_id = -1;
    send_message_to_client(client, MESSAGE_OK, 0,msg->sequence_number,NULL);
}

/*wrapper for send_message_to_socket function of message.h
    handles socket mutex
*/
int send_message_to_client(CLIENT *client, int type, int length, int sequence_number, char *data) {
    int return_value;
    pthread_mutex_lock(&client->socket_write_lock);
    return_value = send_message_to_socket(client->socket, type,length, sequence_number, data);
    pthread_mutex_unlock(&client->socket_write_lock);
    return return_value;
}

void* client_connect(void *client_ds) {
    CLIENT *client = (CLIENT*) client_ds;
    client->nick_is_set = 0;
    client->room_id = -1;

    if (pthread_mutex_init(&(client->socket_write_lock),NULL) != 0)
    {
        printf("ERROR: Init of mutex for client\n");
        exit(-1);
    }


    client->client_next = NULL;
    int exit_cond = 0;

    char *room_names;
    while(!exit_cond) {
        MESSAGE msg;
        int return_value = read_message_from_socket(client->socket, &msg);
        if(return_value == -1) {
            printf("error reading message from client. disconnecting\n");
            exit_cond = 1;
            break;
        }
        if(return_value == -2) {
            exit_cond = 1;
            break;
        }
        printf("message of type %d received from client %d\n", msg.type,client->client_id);
        switch(msg.type) {

            case MESSAGE_NEW_NICK: set_nick(client, &msg);
                                    break;
            case MESSAGE_LIST_ROOMS_REQUEST:
                room_names = get_room_names();
                printf("rooms: %s", room_names);
                send_message_to_client(client,MESSAGE_LIST_ROOMS_REPLY,strlen(room_names),msg.sequence_number,room_names);
                free(room_names);
                break;
            case MESSAGE_CHAT_MSG_TO_SERVER:
                printf("calling chat message handle for client%d\n",client->client_id);
                handle_chat_message(client, &msg);
                break;
            case MESSAGE_ENTER_ROOM:
                handle_enter_room(client, &msg);
                break;
            case MESSAGE_EXIT_ROOM:
                handle_exit_room(client, &msg);
                break;
            default:
                printf("received unknown message¸type %d \n", msg.type);

        }
    }
    disconnect_cleanly(client);

}


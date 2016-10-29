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

void disconnect_cleanly(int client_id, int sock) {
 close(sock);
}

//handles message of type MESSAGE_CHAT_MSG
void handle_chat_message(CLIENT *client, MESSAGE *msg) {} ///TODO
void handle_enter_room(CLIENT *client, MESSAGE *msg) {
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
void handle_exit_room(CLIENT *client, MESSAGE *msg) {}

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

void client_connect(int client_id, int sock) {

    CLIENT client;
    client.client_id = client_id;
    client.socket = sock;
    client.nick_is_set = 0;
    client.room_id = -1;

    if (pthread_mutex_init(&client.socket_write_lock,NULL) != 0)
    {
        printf("ERROR: Init of mutex for client\n");
        exit(-1);
    }


    client.client_next = NULL;
    int exit_cond = 0;

    char *room_names;
    while(!exit_cond) {
        MESSAGE msg;
        int return_value = read_message_from_socket(sock, &msg);
        if(return_value == -1) {
            printf("error reading message from client. disconnecting\n");
            exit_cond = 1;
            break;
        }
        if(return_value == -2) {
            printf("client has disconnected\n");
            exit_cond = 1;
            break;
        }

        switch(msg.type) {
            case MESSAGE_NEW_NICK: set_nick(&client, &msg);
                                    break;
            case MESSAGE_LIST_ROOMS_REQUEST:
                room_names = get_room_names();
                printf("rooms: %s", room_names);
                send_message_to_client(&client,MESSAGE_LIST_ROOMS_REPLY,strlen(room_names),msg.sequence_number,room_names);
                free(room_names);
                break;
            case MESSAGE_CHAT_MSG_TO_SERVER:
                handle_chat_message(&client, &msg);
                break;
            case MESSAGE_ENTER_ROOM:
                handle_enter_room(&client, &msg);
                break;
            case MESSAGE_EXIT_ROOM:
                handle_exit_room(&client, &msg);
                break;
            default:
                printf("received unknown message¸type %d \n", msg.type);

        }
    }
    disconnect_cleanly(client_id,sock);

}


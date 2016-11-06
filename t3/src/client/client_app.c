#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>
#include "../shared/message.h"
#include <pthread.h>
#include "message_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* connection states */
#define DISCONNECTED 0
#define CONNECTED 1
int connection_state;

/*sequence number*/
int sequence_number;

//returns socket descriptor
//error codes:
// -1 could not create socket
// -2 connection error
int connect_to_server(char *ip_address, int port) {
    int socket_desc;
    struct sockaddr_in server;
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        return -1;
    }
    server.sin_addr.s_addr = inet_addr(ip_address);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        return -2;
    }
    connection_state = CONNECTED;
    return socket_desc;
}

//returns 0 if ok
//returns -1 if nick is invalid
int set_nick(int socket_desc, char *nick) {
    int return_send;
    return_send = send_message_to_socket(socket_desc, MESSAGE_NEW_NICK, strlen(nick), sequence_number, nick);
    sequence_number++;
    MESSAGE *ans;
    ans = get_message_from_queue(MESSAGE_CONTROL, 1);

    if(ans->type == MESSAGE_OK) {
        return 0;
    } else if(ans->type == MESSAGE_ERROR_INVALID_NICK)
        return 1;
    else
        printf("error. wrong type of message received when trying to set nick. message type: %d \n", ans->type);
    return -1;
}

int list_rooms(int socket_desc) {
    int return_send = send_message_to_socket(socket_desc, MESSAGE_LIST_ROOMS_REQUEST,0, sequence_number, NULL);
    if(return_send != 0)
        return -1;
    MESSAGE *ans;
    ans = get_message_from_queue(MESSAGE_CONTROL, 1);
    if(ans->type == MESSAGE_LIST_ROOMS_REPLY) {
        printf("rooms available:\n %s \n", ans->data);
    } else {
        printf("an error happened while fetching the room list");
    }
    return 0;
}

void cl_ui(int socket_desc) {
    int exit_client = 0;
    char buffer[1000];
    char command[10];
    char parameter[100];
    while(!exit_client) {
        fgets(buffer, 1000, stdin);
        if(*buffer == '\\') {
            sscanf(buffer,"\\%s %s\n", command, parameter);
            if(strcmp(command, "nick")==0) {
                set_nick(socket_desc,parameter);
            } else if(strcmp(command, "rooms")==0){
                list_rooms(socket_desc);
            } else {
                printf("unknown command\n");
            }

        } else {
            //chat message
        }
    }

}

int main(int argc , char *argv[])
{
    if(argc < 3){
        printf("insuficcient arguments: ./client ip port");
        exit(0);
    }

    int port = atoi(argv[2]);
    connection_state = DISCONNECTED;
    int socket_desc = connect_to_server(argv[1],port);
    if(socket_desc < 0 || connection_state == DISCONNECTED) {
        printf("error while connecting");
        exit(0);
    }
    printf("socket is %d\n", socket_desc);
    sequence_number = 0;
    pthread_t thread_message_listener;
    //starts thread that listens on the socket for new messages
    pthread_create(&thread_message_listener, NULL, listen_for_message, (void*) &socket_desc);

    cl_ui(socket_desc);

    close(socket_desc);
    printf("end");
}

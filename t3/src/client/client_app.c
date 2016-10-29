#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>
#include "../shared/message.h"

int main(int argc , char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    char *message , server_reply[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8880 );

    //Connect to remote server
    if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("connect error\n");
        return 1;
    }
    char name[10] = "teste";
    printf("sending nick \n");
    int value = send_message_to_socket(socket_desc, MESSAGE_NEW_NICK, 10, 1, name);
    MESSAGE msg;
    value = read_message_from_socket(socket_desc, &msg);

    send_message_to_socket(socket_desc, MESSAGE_LIST_ROOMS_REQUEST,0,2,NULL);
    read_message_from_socket(socket_desc,&msg);
    printf("done\n");
    puts(msg.data);
    send_message_to_socket(socket_desc, MESSAGE_ENTER_ROOM,7,3,"sasla1");
    read_message_from_socket(socket_desc,&msg);
    printf("done\n");
    close(socket_desc);
}

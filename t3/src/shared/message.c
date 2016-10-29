#include "message.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

unsigned int get_message_length(char *pointer)
{
    MESSAGE *p = (MESSAGE*) pointer;
    return p->length;
}


int read_message_from_socket(int sock, MESSAGE *msg) {

    char buff[100];
    //reads first 12 bytes (HEADER)
    int nbytes = 0, ret;
    while(nbytes < 12) {
        ret = recv(sock, buff+nbytes, MESSAGE_HEADER_SIZE-nbytes, 0);
        if(ret == -1) {
            //problem with sock
            return -1;
        } else if(ret == 0) {
            //client disconnected
            return -2;
        }
        nbytes += ret;
    }
    msg->type = *((uint32_t*) buff);
    msg->length = *((uint32_t*) (buff+4));
    msg->sequence_number = *((uint32_t*) (buff+8));

    if(msg->length > 0) {
        nbytes = nbytes - MESSAGE_HEADER_SIZE; //accounts for header

        char *payload = (char*) calloc(sizeof(char), msg->length);
        if(nbytes > 0) {
            memcpy(payload, buff+MESSAGE_HEADER_SIZE, nbytes);
        }

        while(nbytes < msg->length) {
            ret = recv(sock, payload+nbytes, msg->length-nbytes, 0);
            if(ret < 0) {
                printf("problem with sock");

                return -1;
            } else if(ret == 0) {
                printf("client disconnected");
                return -2;
            }
            nbytes += ret;
        }
        msg->data = payload;
    } else {
        msg->data = NULL;
    }

    printf("Message of type %d and length %d received with sequence number= %d\n", msg->type, msg->length,msg->sequence_number);

    return 0;
}

int send_message_to_socket(int sock, int type, int length, int sequence_number, char *data) {
        MESSAGE msg;
        msg.type = type;
        msg.length = length;
        msg.sequence_number = sequence_number;
        msg.data = data;


        int bytes = 0, ret;
        while(bytes < 4) {
            ret = send(sock , ((char*)&msg.type)+bytes , sizeof(msg.type)-bytes, 0);
            if(ret == -1) {
                //problem with socket
                printf("send error1\n");
                return -1;
            }
            bytes += ret;
        }
        bytes = 0;
        while(bytes < 4) {
            ret = send(sock , ((char*)&msg.length)+bytes , sizeof(msg.length)-bytes , 0);

            if(ret == -1) {
                //problem with socket
                printf("send error2\n");
                return -1;
            }
            bytes += ret;
        }

        bytes = 0;
        while(bytes < 4) {
            ret = send(sock , ((char*)&msg.sequence_number)+bytes , sizeof(msg.sequence_number)-bytes , 0);

            if(ret == -1) {
                //problem with socket
                printf("send error3\n");
                return -1;
            }
            bytes += ret;
        }
        if(msg.data != NULL) {
            //has to send payload
            bytes = 0;
            while(bytes < msg.length) {
                ret = send(sock , ((char*)msg.data)+bytes , msg.length - bytes , 0);
                if(ret == -1) {
                    //problem with socket
                    printf("send error4\n");
                    return -1;
                }
                bytes += ret;
            }
        }
        return 0;
}

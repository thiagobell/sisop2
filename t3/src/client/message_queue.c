#include "message_queue.h"
#include "../shared/message.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_cond_t new_message;
extern exit_client;
typedef struct message_list {
    MESSAGE msg;
    struct message_list *previous;
    struct message_list *next;
}MSG_LST;

MSG_LST *list;



void add_to_list(MESSAGE msg){
    MSG_LST *temp =(MSG_LST*) calloc(sizeof(MSG_LST),1);
    temp->msg = msg;
    if(list == NULL) {
        //empty
        list = temp;
        list->next = list;
        list->previous = list;
    } else {
        temp->next = list;
        list->previous->next = temp;
        list->previous = temp;
    }
}

MESSAGE* get_from_list(int msg_category) {
    if(list == NULL) {
        return NULL;
    }
    MSG_LST *cursor = list;
    MESSAGE *return_value = NULL;
    do {
        if(get_message_category(cursor->msg.type) == msg_category) {
            return_value = (MESSAGE*)calloc(sizeof(MESSAGE),1);
            *return_value = cursor->msg;

            if(cursor == cursor->next) {
                //list has only this element
                list = NULL;
            } else {
                cursor->previous->next = cursor->next;
                cursor->next->previous = cursor->previous;
            }
            free(cursor);
            break;
        }
        cursor = cursor->next;
    }while(cursor != list);
    return return_value;
}

pthread_mutex_t mutex;
int inited = 0;

MESSAGE* get_message_from_queue(int message_category, int block)
{
    if(!inited) {
        return NULL;
    }
    MESSAGE *return_message;
    int found = 0;
    //acquire mutex
    //printf("trying to acquire mutex on get message\n");
    pthread_mutex_lock(&mutex);
   // printf("mutex acquired on get message\n");
    while((return_message = get_from_list(message_category)) == NULL) {
        //no message to get
        //printf("no message on the queue\n");
        if(block) {
            //printf("blocking\n");
            pthread_cond_wait(&new_message, &mutex);
        } else {
            //printf("not blocking\n");
            return_message = NULL;
            break;
        }
    }
    //printf("unlocking mutex get message\n");
    pthread_mutex_unlock(&mutex);
    return return_message;

}

void handle_incoming_chat_message(MESSAGE msg) {
    printf("%s",msg.data);
    return;
}

//this function is run by its own thread
void* listen_for_message(void *socket_desc_void) {
    int socket_desc = *((int*) socket_desc_void);
    pthread_mutex_init(&mutex, NULL);
    //initialize condition variables
    pthread_cond_init(&new_message,NULL);
    inited = 1;
    MESSAGE temp;
    while(1) {
        int j = 0;
        int value = read_message_from_socket(socket_desc,&temp);
        //printf("acquiring mutex on listen for message\n");
        pthread_mutex_lock(&mutex);
        if(value == 0) {
            //printf("received message of type: %d\n",temp.type);
            if(get_message_category(temp.type) == MESSAGE_CHAT)
                handle_incoming_chat_message(temp);
            else
                add_to_list(temp);
        } else if(value == -2) {
            printf("server has disconnected\n");
            exit_client = 1;
            break;
        }
        //printf("releasing mutex on listen for message\n");
        pthread_mutex_unlock(&mutex);
        if(value == 0)
            pthread_cond_signal(&new_message);
    }
}

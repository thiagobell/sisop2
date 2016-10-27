#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>

/* message types are defined here */
#define MESSAGE_CHAT_MSG
#define MESSAGE_ENTER_ROOM


/* this module defines the message format for the chat application */

typedef struct message {
    uint32_t type; /*message type*/
    uint32_t length; /*length of whole message*/
    char *data; /*payload*/
}MESSAGE;


/* to call this function there must be the first 64 bits of valid data on the buffer
pointer by the functions paramenter. */
unsigned int get_message_length(char *pointer);

#endif // MESSAGE_H

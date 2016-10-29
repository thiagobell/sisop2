#ifndef MESSAGE_H
#define MESSAGE_H
#include <stdint.h>

/* this module defines the message format for the chat application */


/* message types are defined here */

/* contains actual text messages
 *   needs to carry payload
 *  client does not confirm this type of message
*/
#define MESSAGE_CHAT_MSG_TO_SERVER 1
#define MESSAGE_CHAT_MSG_TO_CLIENT 2


/* enter room
    needs payload: room name
    if rooms does not exist, creates a room
*/
#define MESSAGE_ENTER_ROOM 3
/* exit room*/
#define MESSAGE_EXIT_ROOM 4

/* request: list rooms*/
#define MESSAGE_LIST_ROOMS_REQUEST 5
/* reply: list rooms
    carries payload. room names separated by \n
*/
#define MESSAGE_LIST_ROOMS_REPLY 6

/* sets nick*/
#define MESSAGE_NEW_NICK 7
/* if nick is invalid server sends this message*/
#define MESSAGE_ERROR_INVALID_NICK 8

/* request was executued successfully */
#define MESSAGE_OK 9

/* sent when trying to enter room without nick*/
#define MESSAGE_ERROR_NICK_NOT_SET 10
/* sent when trying to enter a full room*/
#define MESSAGE_ERROR_ROOM_IS_FULL 11
/* sent when there is no more space for
additional rooms*/
#define MESSAGE_ERROR_MAX_ROOM_REACHED 11





/* Since the answer to a request may arrive after
 * another message sent by another client through
 * another thread on the server, a sequence number
 * is important to have. A reply with sequence number
 * acknowledges receipt of request with the same number.
*/
typedef struct message {
    uint32_t type; /*message type*/
    uint32_t length; /*length of payload*/
    uint32_t sequence_number; /* message number */
    char *data; /*payload*/
}MESSAGE;

/*size in bytes of message header*/
#define MESSAGE_HEADER_SIZE 12


/* to call this function there must be the first 64 bits of valid data on the buffer
pointer by the functions paramenter. */
unsigned int get_message_length(char *pointer);

int read_message_from_socket(int sock, MESSAGE *msg);
int send_message_to_socket(int sock, int type, int length, int sequence_number, char *data);

#endif // MESSAGE_H

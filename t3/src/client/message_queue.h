#ifndef MESSAGE_QUEUEH
#define MESSAGE_QUEUEH

#include "../shared/message.h"

MESSAGE* get_message_from_queue(int message_type, int block);

void* listen_for_message(void *socket_desc_void);

#endif


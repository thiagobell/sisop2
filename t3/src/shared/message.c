#include "message.h"


unsigned int get_message_length(char *pointer)
{
    MESSAGE *p = (MESSAGE*) pointer;
    return p->length;
}

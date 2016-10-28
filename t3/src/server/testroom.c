#include <stdint.h>
#include "room.h"

#include "client.h"
#include <string.h>
#include <stdio.h>

int main() {
    int value;
    //initializing room
    initialize();
    //creates rooms
    value = create_room("sala1");
    printf("return value of adding room was %d\n", value);
    value = create_room("sala2");
    printf("return value of adding room was %d\n", value);



    //criando clientes
    CLIENT c1, c2;
    c1.client_id = 1;
    strcpy(c1.nick,"cliente 1");
    c2.client_id = 2;
    strcpy(c2.nick,"cliente 2");

    //add client to room
    int ret = add_client_to_room(c1, "sala2");
    printf("return of client add was %d\n",ret);
    ret = add_client_to_room(c2, "sala2");
    printf("return of client add was %d\n",ret);
    ret = add_client_to_room(c1, "sala2");
    ret = add_client_to_room(c2, "sala2");
    printf("return of client add was %d\n",ret);
    //check to see if there is no replicated clietns
    printf("number of clients in room is %d\n", get_num_clients_in_room("sala2"));


    print_clients_in_room("sala2");
    //removing client from room


    remove_client_from_room(2, "sala2");
    print_clients_in_room("sala2");
    remove_client_from_room(2, "sala2");
    print_clients_in_room("sala2");
    printf("number of clients in room is %d\n", get_num_clients_in_room("sala2"));
    printf("test\n");
    add_client_to_room(c2,"sala2");
    print_clients_in_room("sala2");
    remove_client_from_room(1, "sala2");
    remove_client_from_room(2, "sala2");
    print_clients_in_room("sala2");
}

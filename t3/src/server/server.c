#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"client.h"
#include<unistd.h>

int main() {
    int sock;
    initialize();
    create_room("sala1");
    create_room("sal2");

    struct sockaddr_in server, addr_cli;
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
            printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8880 );

    //Bind
    if( bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
       puts("bind failed");
    }
    puts("bind done");
    listen(sock, 1);
    socklen_t size;
    size =sizeof(addr_cli);
    client_connect(1, accept(sock,(struct sockaddr *)&addr_cli, (socklen_t *)&size ));
    close(sock);

}

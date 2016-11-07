#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include"client.h"
#include<unistd.h>
#include<stdlib.h>



int main(int argc , char *argv[]) {

    if(argc < 2){
        printf("insuficcient arguments: ./server.bin port");
        exit(0);
    }
    int port = atoi(argv[1]);
    int client_id=0;
    int sock;
    initialize();


    struct sockaddr_in server, addr_cli;
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
            printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    //Bind
    if( bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
       puts("bind failed");
    }
    puts("bind done");
    listen(sock, 1);
    while(1) {
        CLIENT *cl = (CLIENT*)calloc(sizeof(CLIENT),1);
        client_id++;
        cl->client_id = client_id;
        cl->socket = accept(sock,NULL, NULL );
        pthread_create(&cl->thread, NULL, client_connect,(void*)cl);
    }

    close(sock);

}

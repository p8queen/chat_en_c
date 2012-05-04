#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

#define PORT 5001

void* hiloEscuchaServidor(void *arg){
    int sd = (int)arg; //socket
    char buffer[256];
    int n;
    printf("en hilo sd: [%d]\n", sd);
    int numbytes;
    char buf[254];
    while(1){
        if ( (numbytes=recv(sd,buf,254,0))==-1) {
            printf("error en recvn\n" ); exit(-1); }
        printf("recibi algo\n");
        buf[numbytes]='\0';
        printf("Mensaje del servidor:%s\n",buf);
        
    }
    return NULL;
}

//espera argv[1] --> 127.0.0.1
int main(int argc, char *argv[])
{
    int sd;
    
//    struct hostent *he;
    struct sockaddr_in server;

    if (argc!=2) 
    {
    printf("Usa:[%s]<Direccion IP>\n", argv[1]);
    exit(1);
    }
/*
    if ( (he=gethostbyname(argv[1]) ) == NULL)
    {
        printf("error en gethosbyname\n" );
        exit(1);
    }
*/
    if((inet_aton(argv[1], &server.sin_addr)) == 0){
        perror("ip invalida\n"); exit(1);
    }
    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    //server.sin_addr=*((struct in_addr*) he->h_addr); //ip
    bzero(&(server.sin_zero),0);
    
    if ( (sd=socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        printf("error en socket\n" );
        exit(1);
    }

    
    if (connect(sd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1)
    {
    printf("error en coneccion con socket servidor\n" );
    exit(1);
    }else{
       write(sd,"cliente\n",8); }

    //hilo de escuchaServidor
    pthread_t id;
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0){
        perror("error init"); exit(1); }
    if(pthread_create(&id, &attr,hiloEscuchaServidor,(void *)sd) != 0){
        perror("ERROR create");exit(1); }

    char entradaTeclado[144];
    while(1){
        fgets(entradaTeclado, sizeof(entradaTeclado), stdin);
        write(sd,entradaTeclado,strlen(entradaTeclado));
    }

    close(sd);

    return 0;
}

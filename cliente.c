#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT 5001
#define MAXDATASIZE 254

//espera argv[1] --> 127.0.0.1
int main(int argc, char *argv[])
{
    int fd, numbytes;
    char buf[MAXDATASIZE];
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
    
    if ( (fd=socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        printf("error en socket\n" );
        exit(1);
    }

    
    if (connect(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1)
    {
    printf("error en coneccion con socket servidor\n" );
    exit(1);
    }else{
        write(fd,"hola\n",5);
    }
    char entradaTeclado[144];
    while(1){
        if ( (numbytes=recv(fd,buf,MAXDATASIZE,0))==-1)
        {
            printf("error en recvn\n" );
            exit(-1);
        }
        printf("recibi algo\n");
        buf[numbytes]='\0';
        printf("Mensaje del servidor:%s\n",buf);
        fgets(entradaTeclado, sizeof(entradaTeclado), stdin);
        write(fd,entradaTeclado,strlen(entradaTeclado));
    }

    close(fd);

    return 0;
}

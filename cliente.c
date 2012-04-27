#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

#define PORT 5001
#define MAXDATASIZE 100

//espera argv[1] --> 127.0.0.1
int main(int argc, char *argv[])
{
    int fd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in server;

    if (argc!=2) 
    {
    printf("Usa:[%s]<Direccion IP>\n", argv[1]);
    exit(1);
    }

    if ( (he=gethostbyname(argv[1]) ) == NULL)
    {
        printf("error en gethosbyname\n" );
        exit(1);
    }

    if ( (fd=socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        printf("error en socket\n" );
        exit(1);
    }

    server.sin_family=AF_INET;
    server.sin_port=htons(PORT);
    server.sin_addr=*((struct in_addr*) he->h_addr);
    bzero(&(server.sin_zero),0);

    if (connect(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1)
    {
    printf("error en connect\n" );
    exit(1);
    }

    if ( (numbytes=recv(fd,buf,MAXDATASIZE,0))==-1)
    {
    printf("error en recvn" );
    exit(-1);
    }

    buf[numbytes]='\0';
    printf("Mensaje del servidor:%s\n",buf);
    close(fd);

    return 0;
}

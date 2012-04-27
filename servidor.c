#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>

int main( int argc, char *argv[] )
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR abriendo socket");
        exit(1);
    }
    /* Inicializa estructura */
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5001;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR bind");
         exit(1);
    }

    /* Espera conecciones    */
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    /* Acepta actual conecion  */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, 
                                &clilen);
    if (newsockfd < 0) 
    {
        perror("ERROR en aceptar coneccion");
        exit(1);
    }
    
    /* Connecion establecida */
    /* crear hilo */
    bzero(buffer,256);
    n = read( newsockfd,buffer,255 );
    if (n < 0)
    {
        perror("ERROR leyendo desde socket");
        exit(1);
    }
    printf("Aqui el mensaje: %s\n",buffer);

    /* Write a response to the client */
    n = write(newsockfd,"Obtuve el mensaje\n",18);
    if (n < 0)
    {
        perror("ERROR escribiendo socket\n");
        exit(1);
    }
    return 0; 
}
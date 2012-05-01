#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <pthread.h>

int sockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;

//recibe cant de conecciones por parametro
int main( int argc, char *argv[] )
{
    if (argc<2) {
       perror("se esperaba ./servidor cantConecciones\n");
       exit(1);
    }
    //int sockfd, newsockfd, portno, clilen;
    int cantConecciones = atoi(argv[1]);
    int portno;
    char buffer[256];
    //struct sockaddr_in serv_addr, cli_addr;
    //int  n;

    // primer llamado a socket
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
    listen(sockfd,5); //5 es canidad máxima
    clilen = sizeof(cli_addr);

    /* Acepta actual conecion  */
    /* crear hilo */
    pthread_t ids[5];
    int contadorHilos = 0;
    int newsockfd, n;
        
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, 
                                    &clilen);
        if (newsockfd < 0) 
        {
            perror("ERROR en aceptar coneccion");
            exit(1);
        }
        
        /* Connecion establecida */

        bzero(buffer,256);
        n = read( newsockfd,buffer,255 );
        if (n < 0)
        {
            perror("ERROR leyendo desde socket");
            exit(1);
        }
        printf("Aqui el mensaje: %s\n",buffer);

        /* Write a response to the client */
        n = write(newsockfd,"Obtuve su mensaje\n",18);
        if (n < 0)
        {
            perror("ERROR escribiendo socket\n");
            exit(1);
        }
        close(newsockfd);
    }//end while

//    ids[contadorHilos] = crear_hilos(hiloComunicacion);
  //  contadorHilos++;
    
    //espero finalizacion de hilos
    //pthread_join(ids[0],NULL);
    return 0; 
}

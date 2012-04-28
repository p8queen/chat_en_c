#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <pthread.h>

int sockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;

void* hiloComunicacion(void *argumento){
    int newsockfd, n;
    char buffer[256];
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
    n = write(newsockfd,"Obtuve el mensaje\n",18);
    if (n < 0)
    {
        perror("ERROR escribiendo socket\n");
        exit(1);
    }
    return NULL;
}

pthread_t crear_hilos(void *funcion_asociada){
    pthread_t id;
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0){ 
        perror("error init thread");
        exit(1); }
    if (pthread_create(&id,&attr,funcion_asociada,NULL) != 0) { 
        perror("Error Create thread");
        exit(1); }
    return id;  
}

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
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    /* Acepta actual conecion  */
    /* crear hilo */
    pthread_t ids[5];
    ids[0] = crear_hilos(hiloComunicacion);

    //espero finalizacion de hilos
    pthread_join(ids[0],NULL);
    return 0; 
}

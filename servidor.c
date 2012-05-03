#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

typedef struct stMensajes {
  int socket;
  char mensaje[255];
} stMensajes;

//Nodo es una pila de conecciones
typedef struct nodo{
    int sd;
    char nombre[25];
    struct nodo *siguiente;
}Nodo;

int sockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;

Nodo* crearNodo(int sd, char nombre[25], Nodo *ptr){
    Nodo *nuevo;
    nuevo = (Nodo*)malloc(sizeof(Nodo));
    nuevo->sd = sd;
    strcpy(nuevo->nombre,nombre);
    nuevo->siguiente = ptr;
    return nuevo;
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
    listen(sockfd,5); //5 es canidad máxima
    clilen = sizeof(cli_addr);

    /* Acepta actual conecion  */
    /* crear hilo */
    pthread_t ids[5];
    int contadorHilos = 0;
    int newsockfd, n;
    stMensajes vecSocket[20];
    int z=0;
    Nodo *head, *ptrPila;
    head = NULL;
    char *p;
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, 
                                    &clilen);
        if (newsockfd < 0) {
            perror("ERROR en aceptar coneccion");
            exit(1);
        }
        vecSocket[z].socket = newsockfd; 
        /* Connecion establecida */
        //vecSocket[z].socket = newsockfd;
        
        bzero(buffer,256);
        n = read( newsockfd,buffer,255 );
        if (n < 0){
            perror("ERROR leyendo desde socket");
            exit(1);
        }
        buffer[n] = '\0';
        printf("Aqui el mensaje: %s\n",buffer);
        //si se conecta
        if(strncmp("cini ",buffer,5) == 0){
            p = p+5;
            head = crearNodo(newsockfd,p,head);
            printf("recibi cini de: %s\n",p);
        }else{
            ptrPila = head;
            printf("%s\n", ptrPila->nombre);
        }


        //strcpy(vecSocket[z].mensaje,buffer);

        /* Write a response to the client */
        n = write(newsockfd,"Obtuve su mensaje\n",18);
        if (n < 0)
        {
            perror("ERROR escribiendo socket\n");
            exit(1);
        }
        n = write(vecSocket[0].socket,buffer,strlen(buffer));
        if (n < 0)
        {
            perror("ERROR escribiendo socket\n");
            exit(1);
        }
        z++;
        //close(newsockfd);
    }//end while

    //mensaje al primer cliente
    n = write(vecSocket[0].socket,
        vecSocket[1].mensaje,strlen(vecSocket[1].mensaje));
    if (n < 0)
    {
        perror("ERROR escribiendo socket\n");
        exit(1);
    }

    return 0; 
}

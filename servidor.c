#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

//estructura enviada por el cliente
typedef struct stMensaje{
    char letra;
    char usuario[25];
    char texto[1024];
}stMensaje;

//Nodo es una pila de conecciones
typedef struct nodo{
    int sd;
    char nombre[25];
    struct nodo *siguiente;
}Nodo;

int sockfd, portno;
struct sockaddr_in serv_addr, cli_addr;
Nodo *head;

Nodo* crearNodo(int sd, char nombre[25], Nodo *ptr){
    Nodo *nuevo;
    nuevo = (Nodo*)malloc(sizeof(Nodo));
    nuevo->sd = sd;
    strcpy(nuevo->nombre,nombre);
    nuevo->siguiente = ptr;
    return nuevo;
}

void agregarUsuario(stMensaje stMsj, int sd){
    //agrega usuario a la Pila
    head = crearNodo(sd,stMsj.usuario,head);
}


void listaUsuarios(int sd){
    Nodo* ptr;
    ptr = head;
    //mandar por sd matriz de usuarios
    while(ptr != NULL){
        printf("[%s]\n", ptr->nombre);
        ptr = ptr->siguiente;
    }

}

void interpretaMensaje(stMensaje stMsj, int sd){
    if (stMsj.letra == 'c')
        agregarUsuario(stMsj, sd);
    else if (stMsj.letra == 'e')
        printf("desconectarUsuario(stMsj))\n");
    else if (stMsj.letra == 'u')
        listaUsuarios(sd);
    else
        printf("enviarMensaje(stMsj)\n");

}

void* hiloCliente(void *arg){
    int sd = (int)arg; //socket
    stMensaje stMsj;
    int n;
    printf("en hilo sd: [%d]\n", sd);
    while(1){
        n = read(sd,&stMsj,sizeof(stMsj));
        if (n < 0){
          perror("ERROR leyendo desde socket"); exit(1); }
        
        printf("en hilo letra: [%c] usuario: [%s] mensaje: [%s] \n",
            stMsj.letra,  stMsj.usuario, stMsj.texto);
        interpretaMensaje(stMsj, sd);
        /* respuesta a cliente */
        n = write(sd,"Obtuve su mensaje\n",18);
        if (n < 0){
            perror("ERROR escribiendo socket\n");exit(1);}
    }
    return NULL;
}

void crearHilo(int newsockfd){
    pthread_t id;
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0){
        perror("error init hilo"); exit(1); }
    if(pthread_create(&id, &attr,hiloCliente,(void *)newsockfd) != 0){
        perror("ERROR create hilo");exit(1); }


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
    int clilen = sizeof(cli_addr);

    /* Acepta actual conecion  */
    /* crear hilo */
    pthread_t ids[5];
    int contadorHilos = 0;
    int newsockfd, n;
    head = NULL; //crea pila 
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, 
                                    &clilen);
        if (newsockfd < 0) {
            perror("ERROR en aceptar coneccion");
            exit(1);
        }
        /* Connecion establecida */
        //si se conecta crear nodo, crear hilo
        crearHilo(newsockfd);

    }//end while

    return 0; 
}

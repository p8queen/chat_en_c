#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


//estructura enviada por el cliente
typedef struct stMensaje{
    char letra;
    char usuario[25];
    char ip[15];
    int puerto;
    char texto[1024];
    char destinatario[25];
}stMensaje;

//Nodo es una pila de conecciones
typedef struct nodo{
    int sd;
    char nombre[25];
    char ip[15];
    int puerto;
    struct nodo *siguiente;
}Nodo;

int sockfd, portno;
struct sockaddr_in serv_addr, cli_addr;
Nodo *head;

Nodo* crearNodo(int sd, char nombre[25], char ip[15],int puerto, Nodo *ptr){
    Nodo *nuevo;
    nuevo = (Nodo*)malloc(sizeof(Nodo));
    nuevo->sd = sd;
    strcpy(nuevo->nombre,nombre);
    strcpy(nuevo->ip,ip);
    nuevo->puerto = puerto;
    nuevo->siguiente = ptr;
    return nuevo;
}

void agregarUsuario(stMensaje stMsj, int sd){
    //agrega usuario a la Pila
    head = crearNodo(sd,stMsj.usuario,stMsj.ip,stMsj.puerto,head);
}


void listaUsuarios(int sd){
    Nodo* ptr;
    ptr = head;
    char lista[255*25],*p;
    //bzero(lista,sizeof(lista));
    int i=0;
    p=lista;
    //mandar por sd matriz de usuarios
    while(ptr != NULL){
        printf("[%s]\n", ptr->nombre);
        strcpy((char*)p,ptr->nombre);
        i=strlen(ptr->nombre);
        p = p+i;
        *p='\n';
        p++;
        ptr = ptr->siguiente;
    }
    *p='\0';
    printf("[%s]\n", lista);
    if(write(sd,lista,strlen(lista))<0){
        perror("error al escribir list\n");exit(1);}

}

void desconectarUsuario(stMensaje stMsj, int sd){
    int primero = 1;
    Nodo *ptr, *antp;
    ptr = head;
    while((ptr != NULL) && (strcmp(ptr->nombre,stMsj.usuario) != 0)){
        primero = 0;
        antp = ptr;
        ptr = ptr->siguiente;
    }

    if(primero){
        head = ptr->siguiente;
        free(ptr);
    }else{
        if(ptr != NULL){
        antp = antp->siguiente;
        free(ptr);
        }
    }   
    //cerrar socket de usuario

    //matar hilo
}

int getSocketDestinatario(char destinatario[25]){
    int sdd=-1;
    Nodo* ptr;
    ptr = head;
    while(ptr != NULL){
        printf("sock destinatario: [%d]\n", ptr->sd);
        if(strcmp(destinatario,ptr->nombre)==0){
            sdd = ptr->sd;
            break;
        }
    ptr = ptr->siguiente;
    }
    return sdd;
}

void enviarA_destinatario(stMensaje stMsj){
    int sdd = getSocketDestinatario(stMsj.destinatario);
    
    if(sdd<0){
      printf("error get socket destinatario\n");
      printf("Intente cwit destinatario\n"); 
    }else{    
        printf("%s\n", stMsj.texto);
        int n=write(sdd,stMsj.texto,strlen(stMsj.texto));
        if (n < 0){
                perror("ERROR escribiendo socket\n");exit(1);}
    }            
}

void interpretaMensaje(stMensaje stMsj, int sd){
    if (stMsj.letra == 'c')
        agregarUsuario(stMsj, sd);
    else if (stMsj.letra == 'e'){
        desconectarUsuario(stMsj, sd);
        pthread_exit(NULL);
        }
    else if (stMsj.letra == 'u')
        listaUsuarios(sd);
    else if (stMsj.letra == 't')
        enviarA_destinatario(stMsj);
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
        
        printf("en hilo cliente, letra: [%c] usuario: [%s] ip: [%s] puerto: [%d] mensaje: [%s] destinatario: [%s]\n",
            stMsj.letra,  stMsj.usuario, stMsj.ip, stMsj.puerto, stMsj.texto, stMsj.destinatario);
        interpretaMensaje(stMsj, sd);
        /* respuesta a cliente */
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

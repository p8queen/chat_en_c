#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#define PORT 5001
#define IP "127.0.01"

//socket usado en handler
int sd, puertoUDP; 

typedef struct stMensaje{
    char letra;
    char usuario[25];
    char texto[1024];
    char destinatario[25];
}stMensaje;
stMensaje stMsj;

// UDP
//
char* getNombreArchivo(){
    char buffer[512], *p;
    strcpy(buffer,"$HOME/temp/udp/README.md");
    //lo abro y si no existe marco error
    int i;
    p=buffer;
    for (i = strlen(buffer); i >=0 ; --i){
        if(buffer[i]=='/')
            break;
        
    }
    p=p+i+1;
    return p;

}

void enviarArchivo(int socketUDP, struct sockaddr_in serv_cliente){
    char nombre[255];
    int n,e,f;
    struct Encabezado{
          char nombre[255];
          off_t size;
      }encabezado;  
    int long_cliente=sizeof(serv_cliente);
    strcpy(encabezado.nombre,"/home/gustavo/temp/uno.php");
    //
    int fd = open(encabezado.nombre,O_RDONLY,0777);
     if( fd<0 ) {
    perror("no se pudo abrir el archivo.\n");
    exit(1);
  }
  encabezado.size = lseek(fd,0,SEEK_END);
  printf("tamanio archivo: %ld\n", encabezado.size);
  f=sendto(socketUDP,&encabezado, sizeof(struct Encabezado),
     0, (struct sockaddr *)&serv_cliente, long_cliente);
    if (f<0){ perror("error en sendto desde servidor a cliente\n");
        exit(1); }

  lseek(fd,0,SEEK_SET);
  while( (e = read(fd,nombre,254)) != 0 ){ 
    nombre[e]='\0'; 
    f=sendto(socketUDP,nombre, sizeof(nombre),
     0, (struct sockaddr *)&serv_cliente, long_cliente);
    if (f<0){
        perror("error en sendto desde servidor a cliente\n");
        exit(1);
    }     
  }

   close(fd); // cierro el archivo O_RDONLY


}

// UDP Escucha
void* hiloUDPServer(void *arg){
    //int sd = (int)arg; //socket
    // primer llamado a socket
    int sdudp = socket(AF_INET, SOCK_DGRAM, 0);
    if (sdudp < 0) {
        perror("ERROR abriendo socket");
        exit(1); }
    /* Inicializa estructura */
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    struct sockaddr_in serv_udp, serv_cliente1;
    serv_udp.sin_family = AF_INET;
    serv_udp.sin_addr.s_addr = INADDR_ANY;
    serv_udp.sin_port = htons(puertoUDP);
     
    if (bind(sdudp, (struct sockaddr *) &serv_udp,
                          sizeof(serv_udp)) < 0) {
         perror("ERROR bind udp\n"); exit(1); }

    /* Espera conecciones    */
    //listen(sdudp,5); //5 es canidad máxima
    char buffer[1024]; //puede ser (char *)&estructura
    int long_cliente=sizeof(serv_cliente1);
    //get nombre archivo y size
    struct Encabezado{
          char nombre[255];
          off_t size;
      }encabezado;  
    int e=recvfrom(sdudp,&encabezado,sizeof(struct Encabezado),0,
            (struct sockaddr *) &serv_cliente1,&long_cliente);
        if (e<0){ perror("error en recvfrom\n");exit(1);}
        //crear archivo
    strcat(encabezado.nombre,".copy");
    int fd = open(encabezado.nombre,O_CREAT | O_WRONLY, 0777); // NOTA: si hago un read va a dar error porque estamos abiendolo como O_WRONLY
    if( fd<0 ) { perror("no se pudo abrir el archivo.\n");exit(1); }
    printf("comienzo de transferencia\n");
    int acumulador=0;
    printf("encabezado.size: %ld\n",encabezado.size );
    while(acumulador < encabezado.size){
        e=recvfrom(sdudp,buffer,sizeof(buffer),0,
            (struct sockaddr *) &serv_cliente1,&long_cliente);
        if (e<0){ perror("error en recvfrom\n");exit(1);}
        write(fd,buffer,strlen(buffer));
        acumulador = acumulador + e;
        printf("%d\n",acumulador);
        
    } 
    close(fd);   
    printf("fin de transferencia\n");
    return NULL;
}

int crearHiloUDP(){
    pthread_t id;
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0){
        perror("error init hilo"); exit(1); }
    if(pthread_create(&id, &attr,hiloUDPServer,NULL) != 0){
        perror("ERROR create hilo");exit(1); }
        return id;
}
// UDP FIN
//
void mostrarMenu(){
    printf("Menu\n");

}


void interpretarEntrada(char cadena[1050]){
    //stMsj;
    char *p;
    char primerPalabra[5];
    primerPalabra[4] = '\0';
    // borra '\n' 
    cadena[strlen(cadena)-1]='\0'; 
    strncpy(primerPalabra,cadena,4);
    if (strcmp(primerPalabra,"cini") == 0){
        stMsj.letra = 'c';
        p=cadena;
        p+=5;
        strcpy(stMsj.usuario,(char*)p);
    }
    else if (strcmp(primerPalabra,"cend") == 0)
        stMsj.letra = 'e';
    else if (strcmp(primerPalabra,"list") == 0)
        stMsj.letra = 'u';
    else if (strcmp(primerPalabra,"help") == 0)
        mostrarMenu();
    else if (strcmp(primerPalabra,"cwit") == 0){
        stMsj.letra = 'd';
        p=cadena;
        p+=5;
        strcpy(stMsj.destinatario,(char*)p);
    }
    else{
        stMsj.letra = 't';
        strcpy(stMsj.texto,cadena);
    }
    
}


void* hiloEscuchaServidor(void *arg){
    int sd = (int)arg; //socket
    char buffer[256];
    int n;
    printf("en hilo sd: [%d]\n", sd);
    int numbytes;
    char buf[255];
    while(1){
        if ( (numbytes=recv(sd,buf,254,0))==-1) {
            printf("error en recvn\n" ); exit(-1); }
        printf("recibi algo\n");
        buf[numbytes]='\0';
        printf("Mensaje del servidor:\n%s\n",buf);
        
    }
    return NULL;
}
void handler_cliente(int arg){
    printf("usuario cerro desde consola\n");
    stMsj.letra = 'e';
    strcpy(stMsj.texto,"cerrado desde handler");
    //write(sd,&stMsj,sizeof(stMsj));
    exit(1);
}
//espera argv[1] --> 127.0.0.1
int main(int argc, char *argv[])
{
    puertoUDP = atoi(argv[2]); 
    int hudp = crearHiloUDP(); 
    
    //signals
    signal(SIGINT,&handler_cliente);
//    struct hostent *he;
    struct sockaddr_in server;
    

    if (argc!=3) 
    {
    printf("Usa:[%s]<Direccion IP> <Puerto>\n", argv[1]);
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

    
    if (connect(sd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1){
       perror("error en coneccion con socket servidor\n" ); 
       exit(1); }

    //hilo de escuchaServidor
    pthread_t id;
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0){
        perror("error init"); exit(1); }
    if(pthread_create(&id, &attr,hiloEscuchaServidor,(void *)sd) != 0){
        perror("ERROR create");exit(1); }

    char entradaTeclado[144];
    while(1){
        fgets(entradaTeclado, sizeof(entradaTeclado)-1, stdin);
        interpretarEntrada(entradaTeclado);
        write(sd,&stMsj,sizeof(stMsj));
    }

    close(sd);
    pthread_join(hudp,NULL);
    return 0;
}

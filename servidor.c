#include <stdio.h>
//#include <sys/types.h>   
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
//#include <netdb.h>


int main(int argc, char *argv[])
{
	//AF_INET->TCP/IP; SOCK_STREAM->sec caracteres; 0->default
	int sd=socket(AF_INET,SOCK_STREAM,0); 
	if(sd==-1){
		perror("no se creó el socket \n");
		return 1;
		}
	struct sockaddr_in Direccion; 
	Direccion.sin_family = AF_INET; //igual que socket
	strcpy(Direccion.sun_path, "socket.txt");
	Direccion.sin_port = Puerto->s_port; // 
	Direccion.sin_addr.s_addr =INADDR_ANY; //atiende a cualquier cliente
	
	bind(sd, )
	



	//struct servent *myport;

/*	myport = getservbyname("tpchat","tcp");
	if(myport==NULL){
		perror("getservbyname error\n");
		return 1;
		}
	printf("%d\n",ntohs(myport->s_port));
	*/
/*	struct sockaddr_in Direccion; 
	Direccion.sin_family = AF_INET; //igual que socket
	Direccion.sin_port = Puerto->s_port; // 
	Direccion.sin_addr.s_addr =INADDR_ANY; //atiende a cualquier cliente
	
	//pegar al SO
	if (bind (sd, (struct sockaddr *)&Direccion, sizeof (Direccion)) == -1) 
	{ 
		perror("Error en bind \n"); 
	}

*/	
		
	close(sd);		
	return 0; //exit_success
}

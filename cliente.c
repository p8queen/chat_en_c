#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

struct mensaje {
	char letra;
	char usuario[255];
	char texto[1024];
	};

void menu();
struct mensaje leer_mensajes();

int main(){
	menu();
	struct mensaje elMensaje;
	
	//leer e imprimir mensajes
	do{
	  elMensaje = leer_mensajes();	
	  //ver mensaje
	  printf("letra: [%c], user: [%s], text: [%s]\n",
		elMensaje.letra, elMensaje.usuario, elMensaje.texto);
	} while ('e',elMensaje.letra != 'e' );
	
	return EXIT_SUCCESS;
}

void menu(){
	printf("\nMenu Ayuda de Comandos\n");	
	printf("-----------------------\n\n");	
	printf("cini nickname: iniciar\n");
	printf("cend: finalizar\n");
	printf("list: listado de usuarios conectados\n");
	printf("sfile /path/archivo: envia archivo al otro usuario\n");				
	printf("\n");	
	}

struct mensaje leer_mensajes(){
	char texto[1024];
	char elUsuario[254];
	struct mensaje msm;
	printf("Ingrese mensaje:\n");
	int a = 0;
	do{	  
	  texto[a] = getchar();
	  a++;
	} while (texto[a-1] != '\n');
	texto[a-1]='\0';
	//para cini ususario
	char cini[5];
	int i;
	strncpy(cini,texto,4);
	cini[4]='\0';
	printf("cini texto: [%s] [%s]\n",cini, texto);
	
	if(strcmp("cini",cini) == 0){
		msm.letra = 'c';
		for(i=0;i<(strlen(texto)-5);i++)
			elUsuario[i] = texto[i+5];
		elUsuario[i] = '\0';
		strcpy(msm.usuario,elUsuario);
		}	
	else if(strcmp("cend",texto) == 0)
		msm.letra = 'e';
	else if (strcmp("list",texto) == 0)
		msm.letra = 'u';
	else{ 
		msm.letra = 't';
		strcpy(msm.texto,texto);
		}	
	return msm;
	
}

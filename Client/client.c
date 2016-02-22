#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "clientLibrary.h"

int main (int argc, char *argv[])
{
	//dichiarazioni di variabili
	int sock_fd, serv_port;
	struct sockaddr_in client_sock;
	
	//controllo sugli argomenti
	if (argc!=3)
	{
		perror("Non è stato immesso il giusto numero di argomenti");
		exit(EXIT_FAILURE);
	}
	
	//creazione del socket e connessione al server (dato indirizzo IP e porta da riga di comando)
	//implementare WhoIs?
	
	serv_port=atoi(argv[2]);
	printf("Creo il socket\n");	//debug
	if ((sock_fd=socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		sendError("MAIN: Errore creazione socket");
	}
	
	client_sock.sin_family=AF_INET;
	client_sock.sin_addr.s_addr=inet_addr(argv[1]);
	client_sock.sin_port=htons(serv_port);

	if ((connect (sock_fd, (struct sockaddr*) &client_sock, sizeof(client_sock)))<0)
	{
		sendError("MAIN: Errore di connessione");
	}
	//passaggio stringa da inserire su file al server
	
	signIn (sock_fd);
	
	close (sock_fd);
	return 0;
}

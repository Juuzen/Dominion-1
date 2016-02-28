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

player UTENTE;	//allocata staticamente, dovrei farla dinamica?
int **GAMEFIELD = NULL;	//allocata dinamicamente, siccome dev'essere spesso modificata, cancellata, ricostruita etc.

int main (int argc, char *argv[])
{
	//---DICHIARAZIONI DI VARIABILI---//
	
	int sock_fd, serv_port;	//socket descriptor e porta del server
	struct sockaddr_in client_sock;	//struttura del socket stesso
	int scelta;	//variabile per gestire la scelta dello switch
	int riprova, loginSuccess = 0, nWrite = 0;	//variabili ausiliarie per gestire il ciclo del login
												//nWrite invece serve per effettuare il controllo sulle varie write
	
	//---CONTROLLO SUGLI ARGOMENTI---//

	if (argc!=3)
	{
		perror("Non è stato immesso il giusto numero di argomenti");
		exit(EXIT_FAILURE);
	}
	
	//---CREAZIONE DEL SOCKET E CONNESSIONE AL SERVER---//
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
	
	//-------------SCELTA INIZIALE - LOGIN, REGISTRAZIONE ED USCITA DEFINITIVA-------------//

	GAMEFIELD=allocaMatrice();	//effettua l'allocazione del campo di gioco (va bene qui?)
	GAMEFIELD=pulisciMatrice(GAMEFIELD);	//effettua la pulizia del campo di gioco (come sopra)

	do
	{

		printf("Inserire l'azione da intraprendere\n\n1 - Registrazione\n2 - Login\n3 - Uscita\n\n");

		scelta=getInteger();
		
		switch (scelta)
		{

			//---REGISTRAZIONE---/

			case 1:
				nWrite=write(sock_fd, &scelta, sizeof(int));
				if (nWrite<0)
				{
					sendError("MAIN MENU: ERRORE COMUNICAZIONE REGISTRAZIONE AL SERVER");
				}
				signIn(sock_fd);
				break;
			
				//---LOGIN---/

			case 2:
				do
				{
					riprova=0;
					nWrite=write(sock_fd, &scelta, sizeof(int));
					if (nWrite<0)
					{
						sendError("MAIN MENU: ERRORE COMUNICAZIONE LOGIN A SERVER");
					}
					loginSuccess=logIn(sock_fd);
					if (loginSuccess==0)
					{
						printf("Si vuole riprovare ad effettuare il login?\n\n0 - No\n1 - Si\n");
						riprova=getInteger();
						if (riprova==1)
						{
							printf("Si riproverà ora il login.\n");
						}
					}
				}	while (riprova==1);
				if (loginSuccess==1)
				{
					gameMenu(sock_fd);
				}
				break;
	
				//---USCITA DAL GIOCO---//

			case 3:
				//uscita(sock_fd);
				break;

				//---CASO DI DEFAULT---//

			default:
				printf("Immettere un carattere tra quelli descritti\n");
				break;
			
		}
	}	while (scelta!=3);

	//---CHIUSURA DEL SOCKET---//

	close(sock_fd);	
	
	return 0;
}

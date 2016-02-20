#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "clientLibrary.h"
#define ORDINE 9

//funzioni di supporto, gestione errori ed eventuali handler

void sendError (char *errString)
{
	perror(errString);
	exit(EXIT_FAILURE);
}

int checkUser (char *nomeUtente)
{
	if ((strlen(nomeUtente))!=4)
		return -1;
	else return 0;
}

int checkPass (char *password)
{
	if ((strlen(password))!=10)
		return -1;
	else return 0;
}

//funzioni del menù iniziale

void signIn (int sock_fd)
{
	char stringaServer[20];
	//char rispostaServer[20];
	
	printf("Immettere la stringa da scrivere su server\n");
	do
	{
		scanf("%s", stringaServer);
		if ((strlen(stringaServer))>20)
		{
			printf("Attenzione, la stringa dev'essere di una lunghezza minore di venti caratteri!\n");
		}
	} while ((strlen(stringaServer))>20);
	
	if ((write(sock_fd, stringaServer, strlen(stringaServer)))<0)
	{
		sendError("SIGNIN: Errore scrittura su socket");
	}
	else printf("Stringa inviata con successo!\n");
}

void logIn (int socket_fd)
{
	char nomeUtente[4];
	char password[10];
	
	printf("Immettere il nome utente\n");
	
	do
	{
		scanf("%4s", nomeUtente);
		if ((checkUser(nomeUtente))==-1)
		{
			printf("Errore, il nome utente non deve superare i 4 caratteri, reimmetterlo\n");
		}
	} while ((checkUser(nomeUtente))!=0);
	
	printf("Inserire ora la password!\n");
	
	do
	{
		scanf("%10s", password);
		if ((checkPass(password))==-1)
		{
			printf("Errore, la password non deve superare i 10 caratteri, reimmetterla\n");
		}
	} while ((checkPass(password))!=0);
	
	//parte ora il passaggio al server delle credenziali
	
	if ((write(socket_fd, nomeUtente, 4))<0)
	{
		sendError("LOGIN: ERRORE PASSAGGIO USERNAME AL SERVER");
	}
	
	if ((write(socket_fd, password, 10))<0)
	{
		sendError("LOGIN: ERRORE PASSAGGIO PASSWORD AL SERVER");
	}
}

void spostamento (int socket_fd, player *utente)
{
	char comandoSpostamento;
	char rispostaServer;	//rispostaServer per ora è definito char, ma nulla impedisce possa essere un intero
	
	printf("Comandi di spostamento\n");
	printf("W - in alto\nA - in basso\nS - a sinistra\nD - a destra\n");
	printf("Se invece vuoi smettere di spostarti, premi C\n");
	printf("Ricorda che vanno bene sia lettere minuscole che maiuscole!\n");
	
	do
	{
		printf("Immetti il comando di spostamento\n");	//debug (or not?)
		
		scanf("%c", &comandoSpostamento);
		
		//switch dei comandi
		
		switch (comandoSpostamento)
		{
			
			//spostamento in alto
			
			case 'W':
			case 'w':
				printf("Hai deciso di spostarti in alto\n");	//debug
				if ((write(socket_fd, &comandoSpostamento, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE INVIO COMANDO AL SERVER");
				}
				if ((read(socket_fd, &rispostaServer, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE RICEZIONE RISPOSTA DEL SERVER");
				}
				/*
				 * switch (rispostaServer), da implementare in ogni switch (è brutto a vedersi ma comodo a farsi
				 * devo regolarmi quando saprò cosa manda il server a seconda di ogni eventualità
				 * la legenda sarà messa in calce alla funzione di spostamento
				 */
				break;
			
			//spostamento a sinistra
			
			case 'A':
			case 'a':
				printf("Hai deciso di spostarti a sinistra\n");
				if ((write(socket_fd, &comandoSpostamento, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE INVIO COMANDO AL SERVER");
				}
				if((read(socket_fd, &rispostaServer, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE RICEZIONE RISPOSTA DAL SERVER");
				}
				/*
				 * switch (rispostaServer), da implementare in ogni switch (è brutto a vedersi ma comodo a farsi
				 * devo regolarmi quando saprò cosa manda il server a seconda di ogni eventualità
				 * la legenda sarà messa in calce alla funzione di spostamento
				 */
				break;
				 
			//spostamento in basso
				 
			case 'S':
			case 's':
				printf("Hai deciso di spostarti in basso\n");
				if ((write(socket_fd, &comandoSpostamento, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE INVIO COMANDO AL SERVER");
				}
				if((read(socket_fd, &rispostaServer, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE RICEZIONE RISPOSTA DAL SERVER");
				}
				/*
				 * switch (rispostaServer), da implementare in ogni switch (è brutto a vedersi ma comodo a farsi
				 * devo regolarmi quando saprò cosa manda il server a seconda di ogni eventualità
				 * la legenda sarà messa in calce alla funzione di spostamento
				 */
				break;
			
			//spostamento a destra
			
			case 'D':
			case 'd':
				printf("Hai deciso di spostarti a destra\n");
				if ((write(socket_fd, &comandoSpostamento, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE INVIO COMANDO AL SERVER");
				}
				if((read(socket_fd, &rispostaServer, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE RICEZIONE RISPOSTA DAL SERVER");
				}
				/*
				 * switch (rispostaServer), da implementare in ogni switch (è brutto a vedersi ma comodo a farsi
				 * devo regolarmi quando saprò cosa manda il server a seconda di ogni eventualità
				 * la legenda sarà messa in calce alla funzione di spostamento
				 */
				break;
			
			//uscita dallo switch di spostamento e dal do while della funzione
			
			case 'C':
			case 'c':
				printf("Hai deciso di uscire dallo spostamento\n");
				if ((write(socket_fd, &comandoSpostamento, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE INVIO COMANDO AL SERVER");
				}
				if((read(socket_fd, &rispostaServer, 1))<0)
				{
					sendError("SPOSTAMENTO: ERRORE RICEZIONE RISPOSTA DAL SERVER");
				}
				/*
				 * switch (rispostaServer), da implementare in ogni switch (è brutto a vedersi ma comodo a farsi
				 * devo regolarmi quando saprò cosa manda il server a seconda di ogni eventualità
				 * la legenda sarà messa in calce alla funzione di spostamento
				 */
				break;
				
			//comandi non contemplati quindi caso di default
			
			default:
				printf("Comando non contemplato!\n");
				break;
				
		}
		 
	} while (comandoSpostamento!='c');
}


//funzioni di gestione del campo di gioco (matrice allocata dinamicamente)

int **allocaMatrice()
{
	int i;
	
	//allocazione e creazione della matrice
	int **matrice=(int **)malloc(ORDINE * sizeof(int*));
	
	for (i=0;i<ORDINE;i++)
	{
		matrice[i]=(int*)malloc(ORDINE*sizeof(int));
	}
	
	return matrice;
}

int **pulisciMatrice (int **campoGioco)
{
	int i, j;
	
	for (i=0;i<ORDINE;i++)
	{
		for (j=0;j<ORDINE;j++)
		{
			campoGioco[i][j]=0;
		}
	}
	
	return campoGioco;
}	

void stampaMatrice (int **campoGioco)
{
	int i, j;
	
	for (i=0;i<ORDINE;i++)
	{
		for (j=0;j<ORDINE;j++)
		{
			printf("%d ", campoGioco[i][j]);
		}
		
		printf("\n");
	}
}

void deallocaMatrice (int **campoGioco)
{
	int i;
	
	for (i=0;i<ORDINE;i++)
	{
		free (campoGioco[i]);
	}
	
	free (campoGioco);
}


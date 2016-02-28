#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "clientLibrary.h"
#define RESET 	"\x1B[0m"
#define REDC	"\x1B[31m"
#define GREENC	"\x1B[32m"
#define BLUEC	"\x1B[34m"
#define MAGC	"\x1B[35m"
#define ORDINE 9
#define USER_MAX 4
#define PASS_MAX 10

extern player UTENTE;	//struttura player globale, dichiarata in main.c
extern int **GAMEFIELD;	//matrice di gioco globale, dichiarata in main.c

//---FUNZIONI DI SUPPORTO ED HANDLER---//

void sendError (char *errString)	//gestione di un errore
{
	perror(errString);
	exit(EXIT_FAILURE);
}

int checkUser (char *nomeUtente)	//controllo sulla lunghezza del nome utente, ritorna 0 se il controllo fallisce, 1 se riesce
{
	if ((strlen(nomeUtente))!=4)
		return 0;
	else return 1;
}

int checkPass (char *password)	//controllo sulla lunghezza della password, ritorna 0 se il controllo fallisce, 1 se riesce
{
	if ((strlen(password))!=10)
		return 0;
	else return 1;
}

void inizializzaUtente ()	//inizializza la struttura utente settando i vari campi a zero
{
	printf("Inizializzazione della struttura utente\n");
	UTENTE.xPos=0;
	UTENTE.yPos=0;
	UTENTE.oggTrovati=0;
}

int getInteger ()
{
	int integer;
	while (scanf("%d", &integer)<1)
	{
		scanf("%*[^\n]%*c");
		printf("Inserire un intero valido\n");
	}
	scanf("%*[^\n]%*c");
	return integer;
}

//---FUNZIONI DEL MENÙ DI REGISTRAZIONE, LOGIN ED USCITA---// (DEFINITIVE)

void signIn (int socket_fd)	//registrazione di un nuovo utente
{

	//---DICHIARAZIONE VARIABILI---//

	char nomeUtente[USER_MAX], password[PASS_MAX];	//allocazione di stringhe per memorizzare username e password
	char rispostaServer='e';	//memorizza la risposta del server, settata di base ad 'e'
	int controlloUser=0;	//memorizza l'esito del controllo sull'username, settato di base a 0 (controllo fallito)
	int controlloPass=0;	//memorizza l'esito del controllo sulla password, settata di base a 0 (controllo fallito)
	int nWrite=0, nRead=0;	//variabili di supporto per il controllo sulle read e sulle write
	
	do 	//permette all'utente di inserire l'username e pass desiderati e li invia al server, effettuando un controllo sulla loro validità
	{
		do 	//prende da tastiera il nome utente desiderato
		{
			printf("Immettere il nome utente desiderato: ");
			scanf("%4s", nomeUtente);
			controlloUser=checkUser(nomeUtente);
			if (controlloUser==0)
			{
				printf("\nNome utente sbagliato.\n");
			}
		}	while (controlloUser==0);	//cicla fin quando il controllo sulla lunghezza non è positivo
		printf("L'username è a posto.\n");	//debug
		nWrite=write(socket_fd, nomeUtente, strlen(nomeUtente));	//invia il nome utente al server
		if (nWrite<0)
		{
			sendError("SIGNIN: ERRORE PASSAGGIO NOME UTENTE AL SERVER");
		}
		printf("write fatta\n");	//debug
		if ((read(socket_fd, &rispostaServer, sizeof(char)))<0)
		{
			sendError("SIGNIN: ERRORE RICEZIONE RISPOSTA DA SERVER");
		}
		printf("read fatta\n");	//debug
		if (rispostaServer=='e')
		{
			printf("Nome utente già occupato.\n");
		}
		else if (rispostaServer=='o')
		{
			printf("Registrazione riuscita.\n");
		}
	}	while (rispostaServer=='e');
	
	do
	{
		printf("Inserire ora la password desiderata: ");
		scanf("%10s", password);
		controlloPass=checkPass(password);
		if (controlloPass==0)
		{
			printf("\nAttenzione, la password dev'essere di 10 caratteri\n");
		}
	}	while (controlloPass==0);
	
	if ((write(socket_fd, password, strlen(password)))<0)
	{
		sendError("SIGNIN: ERRORE PASSAGGIO PASSWORD");
	}
}

int logIn (int sock_fd)
{
	int nRead = 0, nWrite = 0, logged = 0, isOkay = 0;
	char nick[USER_MAX + 1], pass[PASS_MAX + 1], response;
	
	do
	{
		printf ("Inserisci il tuo nick, per favore: ");
		scanf ("%4s", nick);
		isOkay = checkUser (nick);
		if (isOkay == 0)
		{
			printf ("\nIl nome utente inserito non è corretto! Riprova.\n");
		}
	} while (isOkay == 0);
	nick[4] = '\0';
	printf ("Il nome utente scelto è: %s\n", nick);
	nWrite = write (sock_fd, nick, USER_MAX);
	if (nWrite < 0)
	{
		sendError ("LOGIN: Errore invio messaggio (1).\n");
	}
	
	isOkay = 0;
	do
	{
		printf ("Ora inserisci la password, per favore: ");
		scanf ("%10s", pass);
		isOkay = checkPass (pass);
		if (isOkay == 0)
		{
			printf ("\nLa password inserita non è corretta! Riprova.\n");
		}
	} while (isOkay == 0);
	pass[10] = '\0';
	printf ("La password scelta è: %s\n", pass);	//debug
	printf ("\n\nLe credenziali sono:\nNick: %s\nPass: %s\n", nick, pass);
	nWrite = write (sock_fd, pass, PASS_MAX);
	if (nWrite < 0)
	{
		sendError ("LOGIN: Errore invio messaggio (2).\n");
	}
	
	nRead = read (sock_fd, &response, 1);
	if (nRead < 0)
	{
		sendError ("LOGIN: Errore ricezione responso (3).\n");
	}
	if (response == 'o')
	{
		printf ("Accesso eseguito!\n");
		logged = 1;
	}
	else
	{
		printf ("Credenziali errate.\n");
	}
	return logged;
}

void uscita (int socket_fd)
{
	int scelta;
	
	printf("Sei davvero sicuro di voler uscire?\n0 - No, 1 - Si\n");
	if (scelta==1)
	{
		printf("Ci vediamo, alla prossima! Grazie per aver giocato al nostro gioco!\n");
		scelta=scelta+2;
		if ((write(socket_fd, &scelta, sizeof(int)))<0)
		{
			sendError("USCITA: ERRORE COMUNICAZIONE SCELTA AL SERVER");
		}
		close (socket_fd);
		exit(EXIT_SUCCESS);
	}
	else if (scelta==2)
	{
		printf("Non vuoi uscire davvero, lo sapevo!\n");
	}
}


//funzioni del menù di gioco

void gameMenu (int socket_fd)
{

	int sceltaMenu, continuaSpost;
	int nWrite = 0;

	printf("Dovrai prima posizionarti sulla matrice\n");
	posizionamento(socket_fd);
	do
	{
		printf("Immettere la scelta del menù di gioco\n");
		printf("1 - spostamento\n");
		sceltaMenu=getInteger();
		nWrite=write(socket_fd, &sceltaMenu, sizeof(int));
		if (nWrite<0)
		{
			sendError("MENU DI GIOCO: ERRORE INVIO SCELTA");
		}
		nWrite=0;
		switch (sceltaMenu)
		{
			case 1:
				do
				{
					stampaMatrice(GAMEFIELD);
					removeOtherUsers();
					sleep(1);
					system("clear");
					stampaMatrice(GAMEFIELD);
					continuaSpost=spostamento(socket_fd);
				}	while (continuaSpost==1);
				break;

			case 2:	//richiesta della classifica
				nWrite=write(socket_fd, &sceltaMenu, sizeof(int));
				if (nWrite<0)
				{
					sendError("MENU DI GIOCO: ERRORE INVIO SCELTA RICHIESTA CLASSIFICA");
				}
				break;

			default:
				printf("Scelta non contemplata.\n");
				break;
		}

	} while (sceltaMenu!=8);
}

int spostamento (int socket_fd)
{
	/*
     * Legenda risposta del server dopo lo spostamento
     * 
     * o - ostacolo
     * u - altro utente
     * m - movimento riuscito
     * t - movimento riuscito e tesoro trovato
     * w - muro
     */

    int cicla = 1, nRead = 0, nWrite = 0;
    char sceltaSpost, rispServer;
    printf("Inserisci la direzione nella quale spostarti.\n");
	printf("W - in alto, A - a sinistra, S - in basso, D - a destra\n");
	scanf(" %c", &sceltaSpost);
	system("clear");
	while (getchar()!='\n');
	nWrite=write(socket_fd, &sceltaSpost, sizeof(char));
	if (nWrite<0)
		sendError("SPOSTAMENTO: Errore invio direzione");
	nRead=read(socket_fd, &rispServer, sizeof(char));
	if (nRead<0)
		sendError("SPOSTAMENTO: Errore ricezione risposta");

	switch (sceltaSpost)
	{
		case 'W':
		case 'w':
			switch (rispServer)
			{
				case 'o':
					GAMEFIELD[(UTENTE.yPos)-1][UTENTE.xPos]=8;
					break;

				case 'u':
					GAMEFIELD[(UTENTE.yPos)-1][UTENTE.xPos]=4;
					break;

				case 'w':
					printf("Hai incontrato un muro\n");
					break;

				case 'm':
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.yPos--;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					break;

				case 't':
					printf("Hai trovato un tesoro!\n");
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.yPos--;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					UTENTE.oggTrovati++;
					break;
			}
			break;

		case 'A':
		case 'a':
			switch(rispServer)
			{
				case 'o':
					GAMEFIELD[UTENTE.yPos][(UTENTE.xPos)-1]=8;
					break;

				case 'u':
					GAMEFIELD[UTENTE.yPos][(UTENTE.xPos)-1]=4;
					break;

				case 'w':
					printf("Ti sei scontrato contro un muro\n");
					break;

				case 'm':
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.xPos--;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					break;

				case 't':
					printf("Hai trovato un tesoro!\n");
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.xPos--;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					UTENTE.oggTrovati++;
					break;
			}
			break;

		case 'S':
		case 's':
			switch(rispServer)
			{
				case 'o':
					GAMEFIELD[(UTENTE.yPos)+1][UTENTE.xPos]=8;
					break;

				case 'u':
					GAMEFIELD[(UTENTE.yPos)+1][UTENTE.xPos]=4;
					break;

				case 'w':
					printf("Ti sei scontrato contro un muro\n");
					break;

				case 'm':
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.yPos++;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					break;

				case 't':
					printf("Hai trovato un tesoro!\n");
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.yPos++;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					UTENTE.oggTrovati++;
					break;
			}
			break;

		case 'D':
		case 'd':
			switch (rispServer)
			{
				case 'o':
					GAMEFIELD[UTENTE.yPos][(UTENTE.xPos)+1]=8;
					break;

				case 'u':
					GAMEFIELD[UTENTE.yPos][(UTENTE.xPos)+1]=4;
					break;

				case 'w':
					printf("Ti sei scontrato contro un muro\n");
					break;

				case 'm':
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.xPos++;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					break;

				case 't':
					printf("Hai trovato un tesoro!\n");
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=1;
					UTENTE.xPos++;
					GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
					UTENTE.oggTrovati++;
					break;
			}
			break;

		case 'C':
		case 'c':
			printf("Uscita dallo spostamento in corso...\n");
			cicla=0;
			break;

		default:
			printf("Caso non contemplato.\n");
			printf("Hai scritto %c\n", sceltaSpost);
			break;
	}
	return cicla;
}


void posizionamento (int socket_fd)
{
	int xInitPos, yInitPos;
	int nRead = 0, nWrite = 0;
	char rispostaServer;
	do
	{
		do
		{
			printf("Inserisci le coordinate iniziali dalle quali vorresti partire\n");
			printf("Cominciamo, per prima l'ascissa. Ricorda che dev'essere compresa tra 1 e 9\n");
			scanf("%d", &xInitPos);
			if (xInitPos<1 || xInitPos>9)
			{
				printf("Attenzione, dev'essere compresa tra uno e nove\nReimmettila pls\n");
			}
		}	while (xInitPos<1 || xInitPos>9);
		xInitPos--;
		nWrite=write(socket_fd, &xInitPos, sizeof(int));
		if (nWrite<0)
		{
			sendError("POSIZIONAMENTO: ERRORE INVIO COORDINATA X A SERVER");
		}
		do
		{
			printf("Inserisci ora la coordinata Y\nRIcorda, compresa tra 1 e 9\n");
			scanf("%d", &yInitPos);
			if (yInitPos<1 || yInitPos>9)
			{
				printf("Attenzione, la Y dev'essere compresa tra uno e nove\n");
			}
		} while (yInitPos<1 || yInitPos>9);
		yInitPos--;
		nWrite=write(socket_fd, &yInitPos, sizeof(int));
		if (nWrite<0)
		{
			sendError("POSIZIONAMENTO: ERRORE INVIO COORD. Y");
		}
		nRead=read(socket_fd, &rispostaServer, 1);
		if (nRead<0)
		{
			sendError("POSIZIONAMENTO: ERRORE RICEZIONE RISPOSTA SERVER COORD Y");
		}
		if (rispostaServer=='e')
		{
			printf("La coordinata da te inserita non è valita");
		}
	} while (rispostaServer=='e');

	if (rispostaServer=='o')
	{
		UTENTE.xPos=xInitPos;
		UTENTE.yPos=yInitPos;
		GAMEFIELD[UTENTE.yPos][UTENTE.xPos]=3;
	}
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

		if (i==0)
		{
			printf("\n ___________________\n");
		}
		printf("|");
		for (j=0;j<ORDINE;j++)
		{
			stampaElemento(campoGioco[i][j]);
		}
		printf("|\n");
		if (i==ORDINE-1)
		{
			printf("¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\n");
		}
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

void stampaElemento (int elem)
{
	switch (elem)
	{	
		case 0:		//posizione sconosciuta
			printf("? ");
			break;

		case 1:		//posizione conosciuta
			printf("  ");
			break;

		case 3:		//player
			printf(BLUEC "U " RESET);	//blu
			break;

		case 4:		//altro player
			printf(MAGC "U " RESET);	//magenta
			break;

		case 5:		//tesoro
			printf(GREENC "T " RESET);	//verde pisello
			break;

		case 8:		//ostacolo
			printf(REDC "X " RESET);	//rosso
			break;
	}
}

void removeOtherUsers ()
{
	int i, j;

	for (i=0;i<ORDINE;i++)
	{
		for (j=0;j<ORDINE;j++)
		{
			if ((GAMEFIELD[i][j])==4)
			{
				GAMEFIELD[i][j]=0;
			}
		}
	}
}
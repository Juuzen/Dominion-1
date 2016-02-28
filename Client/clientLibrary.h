#ifndef CLIENTLIBRARY_H
#define CLIENTLIBRARY_H

struct giocatore {
	int xPos;
	int yPos;
	int oggTrovati;
};

/* Nota per me, le matrici in C sono allocate in questo modo

	(0,0) (0,1) (0,2)

	(1,0) (1,1) (1,2)

	(2,0) (2,1) (2,2)

Quindi se io mi trovo in posizione (1,1) per effettuare uno spostamento in alto devo diminuire la X di uno, portandomi quindi a (0,1)


*/

typedef struct giocatore player;

//funzioni di supporto, gestione errori, eventuali handler

void sendError (char *errString);	//richiama perror ed esce dal processo
int checkUser (char *nomeUtente);	//controlla la lunghezza dell'username
int checkPass (char *password);		//controlla la lunghezza della password
void inizializzaUtente ();	//funzione che inizializza la struct utente, per evitare di andare a scrivere
							//in campi con valori non legali (esempio, oggetti trovati non settati inizialmente a zero)
int getInteger ();			//funzione per acquisire un intero da tastiera e pulire lo stdin

//funzioni per il menù di registrazione/login

void signIn (int socket_fd);	//funzione di registrazione di un account
int logIn (int socket_fd);		//funzione di login al server
void uscita (int socket_fd);	//funzione di uscita dal gioco (menù principale del main)

//funzioni del menù di gioco

void gameMenu (int socket_fd);	//gestione del menù di gioco vero e proprio
int spostamento (int socket_fd);	//funzione di spostamento di un giocatore sulla mappa
void posizionamento (int socket_fd);	//funzione che permette il posizionamento di un player sulla mappa di gioco

//funzioni di gestione della matrice di gioco lato client

int **allocaMatrice ();
int **pulisciMatrice(int **campoGioco);
void stampaMatrice (int **campoGioco);
void deallocaMatrice (int **campoGioco);
void stampaElemento (int elem);
void removeOtherUsers ();


#endif //CLIENTLIBRARY_H

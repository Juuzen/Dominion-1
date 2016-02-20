#ifndef CLIENTLIBRARY_H
#define CLIENTLIBRARY_H

struct giocatore {
	int x;
	int y;
	int oggTrovati;
};

typedef struct giocatore player;

//funzioni di supporto, gestione errori, eventuali handler

void sendError (char *errString);	//richiama perror ed esce dal processo
int checkUser (char *nomeUtente);	//controlla la lunghezza dell'username
int checkPass (char *password);		//controlla la lunghezza della password

//funzioni per il menù di registrazione/login

void signIn (int socket_fd);	//funzione di registrazione di un account
void logIn (int socket_fd);		//funzione di login al server
void spostamento (int socket_fd, player *utente);	//funzione di spostamento di un giocatore sulla mappa

//funzioni di gestione della matrice di gioco lato client

int **allocaMatrice ();
int **pulisciMatrice(int **campoGioco);
void stampaMatrice (int **campoGioco);
void deallocaMatrice (int **campoGioco);



#endif //CLIENTLIBRARY_H

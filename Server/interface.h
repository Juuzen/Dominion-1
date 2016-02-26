#ifndef interface_h
#define interface_h
#define NICKMAX 5
#define PASSMAX 11

// Struttura dati DB
struct user
{
  char *nickname;
  char *password;
  struct user *next;
};
typedef struct user User;
typedef User *PUser;

// Struttura dati giocatore
struct player
{
	char nickname[NICKMAX]; // Nickname del giocatore (per una più facile individuazione)
    int x; // Posizione X sulla mappa di gioco
    int y; // Posizione Y sulla mappa di gioco
    char ip[256]; // indirizzo IP del giocatore
    int isPlaying; // Flag per determinare se un giocatore è in gioco oppure no (realmente utile?)
    int sock_fd; // File descriptor del socket di comunicazione client-server
    int score; // Numero di tesori trovati dal giocatore
    struct player *next; // Puntatore al prossimo elemento della lista
};
typedef struct player Player;
typedef Player* PPlayer;

// Struttura dati campo di gioco
typedef int** gamefield;

// Supporto
void sendError (char *message);
void checkArguments (int argc, int argnum);
int openSocket ();
void sv_shutdown ();
void *cl_run (void *arg_sockfd);

// gestione giocatori
PPlayer allocatePlayer ();
void deallocatePlayer (PPlayer pl);
PPlayer addPlayer (PPlayer top, PPlayer pl);
PPlayer removePlayer (PPlayer pl, char *nick);
void resetPlayerList (PPlayer top);
PPlayer deallocatePlayerList (PPlayer top);

// Funzioni gestione struttura db
PUser allocateUser ();
void deallocateUser (PUser us);
void deallocateUserList (PUser userList);
void printUserList (PUser userList); // only for testing
PUser setUser (char *nick, char *pass);
PUser loadUser (PUser userList, PUser us);
PUser newUser (PUser userList, char *nick, char *pass);

// Funzioni gestione DB
int openDB (const char *db_name, int mode);
void closeDB (int db_fd);
PUser loadDB (const char *db_name);
void writeUser (const char *db_name, char *nick, char *pass);

// Funzioni gestione campo di gioco
gamefield allocateGF ();
void deallocateGF (gamefield gf);
void printGF (gamefield gf); // only for testing
void cleanGF (gamefield gf);
void setObstacle (gamefield gf);
void setTreasure (gamefield gf);
void setGF (gamefield gf);

// Gestione partita
int nickIsAvailable (PUser top, char *nick);
void signup (int sock_fd, PUser ul, const char *db_filename);
PUser findUser (PUser top, char *nick);
int signin (PPlayer pl, PUser ul);

#endif

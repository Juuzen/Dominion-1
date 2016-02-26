#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "interface.h"
#define NICKMAX 5
#define PASSMAX 11

int const order = 9;
int const treasureMax = 10;
int const obstacleMax = 10;

extern int SOCKFD;
extern void *status;
extern PPlayer PLAYERLIST;
extern PUser USERLIST;
extern const char DBNAME[];

//------ Supporto ------//
void sendError (char *message)
{
  perror (message);
  exit (EXIT_FAILURE);
}

void checkArguments (int argc, int argnum)
{
  if (argc != argnum)
  {
    sendError ("CHECKARGUMENTS: Errore nel numero di parametri (1).\n");
  }
  printf ("Numero di argomenti corretto.\n");
}

int openSocket ()
{
  int fd;
  fd = socket (PF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    sendError ("OPENSOCKET: Errore apertura socket (1).\n");
  }
  return fd;
}

void sv_shutdown ()
{
  printf ("Chiusura server.\n");
  if ((close (SOCKFD)) < 0)
  {
    sendError ("SHUTDOWN: Errore chiusura socket (1).\n");
  }
  deallocateUserList (USERLIST);
  deallocatePlayerList (PLAYERLIST);
}

void *cl_run (void *arg)
{
  PPlayer pl = (PPlayer) arg, playerlist = PLAYERLIST;
  int nRead = 0, nWrite = 0, scelta = 0, logged = 0;

  do
  {
    nRead = read (pl->sock_fd, &scelta, sizeof (int));
    if (nRead < 0)
    {
      sendError ("CLIENT: Errore in lettura (1).\n");
    }
    else if (nRead == 0)
    {
      close (pl->sock_fd);
      pthread_exit (status);
    }
    else
    {
      if (scelta == 1) // Registrazione
      {
        signup (pl->sock_fd, USERLIST, DBNAME);
        printf ("\n\n\n\n\n\n\nIl database:\n");
        printUserList (USERLIST);
      }
      else if (scelta == 2) // Login
      {
		logged = signin (pl, USERLIST);
		if (logged == 1)
		{
			// viva il gioco
		}
      }
      else if (scelta == 3) // Chiusura programma
      {
		break;
      }
      else
      {
        continue;
      }
    }
  } while(scelta != 3);
  close (pl->sock_fd);
  pthread_exit (status);
}

//------ Gestione struttura giocatori ------//
PPlayer allocatePlayer ()
{
    PPlayer tmp = (PPlayer) malloc (sizeof (Player));
    tmp->x = -2;
    tmp->y = -2;
    tmp->score = 0;
    tmp->isPlaying = 0;
    tmp->next = NULL;
    return tmp;
}

void deallocatePlayer (PPlayer pl)
{
  free (pl);
}

PPlayer addPlayer (PPlayer top, PPlayer pl)
{
  if (top == NULL) return pl;
  else
  {
    top->next = addPlayer (top->next, pl);
    return top;
  }
}

PPlayer removePlayer (PPlayer top, char *nick)
{
  if (top != NULL)
  {
    int tmp = strcmp (top->nickname, nick);
    if (tmp == 0)
    {
      PPlayer tmp_pl = top->next;
      deallocatePlayer (top);
      return tmp_pl;
    }
    else top->next = removePlayer (top->next, nick);
  }
  return top;
}

void resetPlayerList (PPlayer top)
{
  if (top != NULL)
  {
    while (top != NULL)
    {
      top->x = -2;
      top->y = -2;
      top->isPlaying = 0;
      top->score = 0;
      top = top->next;
    }
  }
}

PPlayer deallocatePlayerList (PPlayer top)
{
  if (top != NULL)
  {
    top->next = deallocatePlayerList (top->next);
    free (top);
    return NULL;
  }
  else return NULL;
}

//------ Funzioni gestione struttura db ------//
PUser allocateUser ()
{
  PUser tmp = (PUser) malloc (sizeof (User));
  tmp->nickname = (char *) calloc (NICKMAX, sizeof (char));
  tmp->password = (char *) calloc (PASSMAX, sizeof (char));
  tmp->next = NULL;
  return tmp;
}

void deallocateUser (PUser us)
{
  free (us->nickname);
  free (us->password);
  free (us);
}

void deallocateUserList (PUser userList)
{
  if (userList != NULL)
  {
    deallocateUserList (userList->next);
    userList->next = NULL;
    deallocateUser (userList);
  }
}

void printUserList (PUser userList) // only for testing
{
  if (userList != NULL)
  {
    printf ("Il nick è: %s\n", userList->nickname);
    printf ("La pass è: %s\n\n", userList->password);
    printUserList (userList->next);
  }
  else printf("cazzo\n");
}

PUser setUser (char *nick, char *pass)
{
  PUser tmp = allocateUser ();
  //printf ("setuser: %s %s\n", nick, pass);
  if ((strlen (nick)) != 4)
  {
    sendError ("SETUSER: nickname in ingresso non di 4 caratteri.");
  }
  else if ((strlen (pass)) != 10)
  {
    sendError ("SETUSER: password in ingresso non di 10 caratteri.");
  }
  else
  {
    strcpy (tmp->nickname, nick);
    tmp->nickname[4] = '\0';
    strcpy (tmp->password, pass);
    tmp->password[10] = '\0';
  }
  return tmp;
}

PUser loadUser (PUser userList, PUser us)
{
  if (userList == NULL)
    return us;

  else
  {
    userList->next = loadUser (userList->next, us);
    return userList;
  }
}

PUser newUser (PUser userList, char *nick, char *pass)
{
  PUser list = userList;
  //printf ("newuser: %s %s\n", nick, pass);
  PUser tmp = setUser (nick, pass);
  list = loadUser (list, tmp);
  return list;
}

//------ Funzioni gestione db ------//
int openDB (const char *db_name, int mode)
{
  int fd;
  if (mode == 0)
  {
    fd = open (db_name, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
      sendError ("OPENDB: Errore apertura file (1).\n");
    }
    printf ("File aperto con successo.\n");
  }
  else if (mode == 1)
  {
    fd = open (db_name, O_WRONLY, S_IWUSR);
    if (fd < 0)
    {
      sendError ("OPENDB: Errore apertura file (2).\n");
    }
    printf ("File aperto con successo.\n");
  }
  else if (mode == 2)
  {
    fd = open (db_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
      sendError ("OPENDB: Errore apertura file (3).\n");
    }
    printf ("File aperto con successo.\n");
  }
  else
  {
    sendError ("OPENDB: Scelta apertura file non valida (4).\n");
  }
  return fd;
}

void closeDB (int db_fd)
{
  if ((close (db_fd)) < 0)
  {
    sendError ("CLOSEDB: Errore chiusura file (1).\n");
  }
  printf ("File chiuso con successo.\n");
}

PUser loadDB (const char *db_name)
{
  int nRead, db_fd;
  char nickbuf[NICKMAX], passbuf[PASSMAX];
  PUser userList = NULL;                                            // Inizialmente il database è vuoto
  db_fd = openDB (db_name, 0);
  do
  {
    nRead = read (db_fd, nickbuf, 4);
    if (nRead < 0)                                                  // Errore nella read
    {
      sendError ("LOADDB: Errore lettura da file (1).\n");
    }
    else if (nRead < 2)                                            // EOF
    {
		printf ("Lettura finita!\n");
      break;                                                        // Esce direttamente dal ciclo, vera unica condizione che permette al ciclo di non andare in loop
    }
    else                                                            // Salvataggio stringa nickname
    {
      nickbuf[4] = '\0';                                            // Dopo la lettura del nick, la stringa viene completata inserendo '\0' alla fine
      printf ("Ho letto il nick: %s\n", nickbuf);                   // printf di sonda
    }

    nRead = read (db_fd, passbuf, 10);
    if (nRead < 0)                                                 // Errore nella read e/o EOF (il database non dovrebbe essere EOF quando si legge una password)
    {
      sendError ("LOADDB: Errore lettura da file (2).\n");
    }
    else if (nRead == 0)
    {
      break;
    }
    else                                                            // Salvataggio stringa password
    {
      passbuf[10] = '\0';                                           // Dopo la lettura della pass, la stringa viene completata inserendo '\0' alla fine
      printf ("Ho letto la password: %s\n\n", passbuf);             // printf di sonda
      userList = newUser (userList, nickbuf, passbuf);              // Inserimento di un nuovo utente nella lista
      printf ("Utente inserito correttamente.\n");
      if ((lseek (db_fd, 1, SEEK_CUR)) < 0)
      {
        sendError ("LOADDB: Errore lettura da file (3).\n");
      }
    }
  } while (nRead > 0);                                              // Se eventualmente l'ultima read() porta a EOF, si esce automaticamente
  closeDB (db_fd);
  return userList;                                                  // Ritorna la lista così creata
}

void writeUser (const char *db_name, char *nick, char *pass)
{
  int nWrite = 0, db_fd = openDB (db_name, 1);
  lseek (db_fd, 0, SEEK_END);
  nWrite = write (db_fd, nick, strlen (nick));
  if (nWrite < 0)
  {
    sendError ("WRITEUSER: Errore scrittura su file (1).\n");
  }
  nWrite = write (db_fd, pass, strlen (pass));
  if (nWrite < 0)
  {
    sendError ("WRITEUSER: Errore scrittura su file (2).\n");
  }
  nWrite = write (db_fd, "\n", 1);
  if (nWrite < 0)
  {
    sendError ("WRITEUSER: Errore scrittura su file (3).\n");
  }
  closeDB (db_fd);
}

//------ Funzioni gestione campo di gioco ------//
gamefield allocateGF ()
{
    int i;
    // si alloca un array di puntatori ad int
    gamefield tmp = (gamefield) calloc (order, sizeof (int *));
    for (i = 0; i < order; i++)
    {
      // si alloca, uno per volta, un array di int
      tmp[i] = (int *) calloc (order, sizeof (int));
    }
    return tmp;
}

void deallocateGF (gamefield gf)
{
    int i;
    for (i = 0; i < order; i++)
        free (gf[i]); // ogni riga viene deallocata
    free (gf); // quindi anche l'array di puntatori viene deallocato
}

void printGF (gamefield gf) //Only for testing
{
    int i, j;
    for (i = 0; i < order; i++)
    {
        for (j = 0; j < order; j++)
            printf ("%d ", gf[i][j]); // stampa il contenuto della matrice
        printf ("\n");
    }
}

void cleanGF (gamefield gf)
{
    int i, j;
    for (i = 0; i < order; i++)
        for (j = 0; j < order; j++)
            gf[i][j] = 0; // tutte le caselle diventano vuote
}

void setObstacle (gamefield gf)
{
    srand (time (NULL));
    int ob = 0;
    int x, y;
    do
    {
        x = rand () % order; // generazione coordinata x
        y = rand () % order; // generazione coordinata y
        if (gf[x][y] == 0) // se la casella è vuota...
        {
            gf[x][y] = 1; // ...allora vi sarà un ostacolo
            ob++; // viene incrementato il numero degli ostacoli
        }
    } while (ob < obstacleMax); // fintanto che il numero degli ostacoli non raggiunge il tetto massimo
}

void setTreasure (gamefield gf)
{
    srand (time (NULL));
    int tr = 0;
    int x, y;
    do
    {
        x = rand () % order; // generazione coordinata x
        y = rand () % order; // generazione coordinata y
        if (gf[x][y] == 0) // se la casella è vuota...
        {
            gf[x][y] = 2; // ...allora vi sarà un tesoro
            tr++; // viene incrementato il numero dei tesori
        }
    } while (tr < treasureMax); // fintanto che il numero dei tesori non raggiunge il tetto massimo
}

void setGF (gamefield gf)
{
    // Pulizia della matrice
    cleanGF (gf);

    // Generazione casuale ostacoli
    setObstacle (gf);

    //Generazione casuale tesori
    setTreasure (gf);
}


//------ Gestione partita ------//

int nickIsAvailable (PUser top, char *nick)
{
  int available = 1;
  int tmp;
  printf ("nickisavailable: %s\n", nick);
  if (top != NULL)
  {
    while ((available == 1) && (top != NULL))
    {
      tmp = strcmp (top->nickname, nick);
      if (tmp == 0)
      {
        available = 0;
      }
      else
      {
        top = top->next;
      }
    }
  }
  return available;
}

void signup (int sock_fd, PUser ul, const char *db_filename)
{
  char nickbuf[NICKMAX], passbuf[PASSMAX];
  int nRead = 0, nWrite = 0, isAvailable = 0;
  do
  {
    nRead = read (sock_fd, nickbuf, NICKMAX - 1);
    if (nRead < 0)
    {
      sendError ("SIGNUP: Errore lettura (1).\n");
    }
    else if (nRead == 0)
    {
      close (sock_fd);
      pthread_exit (status);
    }
    nickbuf[4] = '\0';
	//printf("Cacca: %s\n", nickbuf);
	//printf("Controllo username\n");
    isAvailable = nickIsAvailable (ul, nickbuf);
    if (isAvailable == 0)
    {
		printf("username non disponibile\n");
      nWrite = write (sock_fd, "e", 1);
      if (nWrite < 0)
      {
        sendError ("SIGNUP: Errore scrittura (2).\n");
      }
    }
	else
	{
		printf ("username disponibile: %s\n", nickbuf);
		nWrite = write (sock_fd, "o", 1);
		if (nWrite < 0)
		{
			sendError ("SIGNUP: Errore scrittura (3).\n");
		}
	}
  } while(isAvailable == 0);

	printf("Lettura password\n");
  nRead = read (sock_fd, passbuf, PASSMAX - 1);
  if (nRead < 0)
  {
    sendError ("SIGNUP: Errore lettura (3).\n");
  }
  else if (nRead == 0)
  {
    close (sock_fd);
    pthread_exit (status);
  }
	passbuf[10] = '\0';
	//printf ("Pipi: %s\n", passbuf);
	printf("Scrittura nel database: %s %s\n", nickbuf, passbuf);
  ul = newUser (ul, nickbuf, passbuf);
  writeUser (DBNAME, nickbuf, passbuf);
}

PUser findUser (PUser top, char *nick)
{
  PUser found = NULL;
  int tmp;
  if (top != NULL)
  {
    while ((found == NULL) && (top != NULL))
    {
      tmp = strncmp (top->nickname, nick, NICKMAX - 1);
      if (tmp == 0)
      {
        found = top;
      }
      else
      {
        top = top->next;
      }
    }
  }
  return found;
}

int signin (PPlayer pl, PUser ul)
{
  int nRead = 0, nWrite = 0;
  int logged = 0, sameNick = 0, samePass = 0;
  char nickbuf[NICKMAX], passbuf [PASSMAX];
  PUser tmp = NULL;


  nRead = read (pl->sock_fd, nickbuf, (NICKMAX - 1));
  if (nRead < 0)
  {
    sendError ("SIGNIN: Errore lettura (1).\n");
  }
  else if (nRead == 0)
  {
    close (pl->sock_fd);
    pthread_exit (status);
  }
	nickbuf[4] = '\0';
		printf("Letto nome utente: %s\n", nickbuf);


  nRead = read (pl->sock_fd, passbuf, (PASSMAX - 1));
  if (nRead < 0)
  {
    sendError ("SIGNIN: Errore lettura (2).\n");
  }
  else if (nRead == 0)
  {
    close (pl->sock_fd);
    pthread_exit (status);
  }
	passbuf[10] = '\0';
	printf("Letta password: %s\n", passbuf);
  tmp = findUser (ul, nickbuf);
  if (tmp == NULL)
  {
    nWrite = write (pl->sock_fd, "e", 1);
    if (nWrite < 0)
    {
      sendError ("SIGNIN: Errore scrittura (1).\n");
    }
  }
  else
  {
    sameNick = strncmp (tmp->nickname, nickbuf, NICKMAX - 1);
    samePass = strncmp (tmp->password, passbuf, PASSMAX - 1);
    if ((sameNick == 0) && (samePass == 0))
    {
printf("Corrispondenza trovata\n");
      logged = 1;
      strcpy (pl->nickname, nickbuf);
      pl->nickname[4] = '\0';
		nWrite = write (pl->sock_fd, "o", 1);
		if (nWrite < 0)
		{
			sendError ("SIGNIN: Errore scrittura (2).\n");
		}
    }
    else
    {
printf("Corrispondenza non trovata\n");
      nWrite = write (pl->sock_fd, "e", 1);
      if (nWrite < 0)
      {
        sendError ("SIGNIN: Errore scrittura (2).\n");
      }
    }
  }
  return logged;
}

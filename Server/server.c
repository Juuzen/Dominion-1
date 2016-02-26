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

int SOCKFD;
void *status;
PPlayer PLAYERLIST = NULL;
PUser USERLIST = NULL;
const char DBNAME[] = "db.txt";

int main (int argc, char *argv[])
{
  int sock_desc, cl_sock, cl_len;
  int serv_port;
  struct sockaddr_in server, client;

  // controllo argomenti
  checkArguments (argc, 2);

  // caricamento database
  USERLIST = loadDB (DBNAME);
  printUserList (USERLIST);
  
  // creazione socket
  sock_desc = openSocket ();
  serv_port=atoi (argv[1]);

  // preparazione struttura sockaddr_in
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(serv_port);

  // bind
  if (bind (sock_desc, (struct sockaddr *) &server, sizeof (server)) < 0)
  {
    sendError ("MAIN: Errore bind.\n");
  }
  printf ("Bind effettuato.\n");

  // listening
  listen (sock_desc, 5);


  // accept
  printf ("In attesa di nuova connessioni...\n\n");
  cl_len = sizeof (struct sockaddr_in);

  while (1)
  {
    cl_sock = accept (sock_desc, (struct sockaddr *) &client, (socklen_t *) &cl_len);
    if (cl_sock < 0)
    {
      sendError ("AVVIO: Connessione non accettata.\n");
    }
    printf ("Connessione accettata.\n");

    // Generazione giocatore
    PPlayer newpl = allocatePlayer ();
    PLAYERLIST = addPlayer (PLAYERLIST, newpl);
    char *ip = (char *) calloc (256, sizeof (char));
    inet_ntop (AF_INET, &client.sin_addr, ip, sizeof (ip));
    strcpy (newpl->ip, ip);
    free (ip);
    newpl->sock_fd = cl_sock;

    pthread_t tid;
    if (pthread_create (&tid, NULL, cl_run, (void *) newpl) < 0)
    {
      sendError ("MAIN: Errore creazione thread.\n");
    }
    pthread_detach (tid);
  }
  close (sock_desc);
  return 0;
}

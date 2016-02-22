#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

void getWhoIs (char *ip, char **data);
int whoIsQuery (char *server, char *query, char **response);
int hostname_to_ip (char *hostname, char *ip);

void getWhoIs (char *ip, char **data)
{
	char *wch=NULL, *pch, *response=NULL;
	
	if (whoIsQuery("whois.iana.org", ip, &response))
	{
		printf("Query fallita\n");
	}
	
	pch=strtok(response, "\n");
	
	while (pch!=NULL)
	{
		wch=strstr(pch, "whois.");
		if (wch!=NULL)
		{
			break;
		}
		
		pch=strtok(NULL, "\n");
	}
	
	if (wch!=NULL)
	{
		printf("Il server di WhoIs e': %s ", wch);
		whoIsQuery(wch, ip, data);
	}
	else
	{
		*data=malloc(100);
		strcpy(*data, "No whois data");
	}
}

int whoIsQuery (char *server, char *query, char **response)
{
	char ip[32], message[100], buffer[1500];
	int sock_fd, read_size, total_size=0;
	struct sockaddr_in dest;
	
	sock_fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	
	printf("Resolving %s...", server);
	if (hostname_to_ip(server, ip))
	{
		printf("Failed\n");
		return 1;
	}
	printf("%s", ip);
	dest.sin_addr.s_addr=inet_addr(ip);
	dest.sin_port = htons(43);
	
	if (connect (sock_fd, (const struct sockaddr*) &dest, sizeof(dest))<0)
	{
		perror("CONNECTION FAILED");
	}
	
	printf("\nQuery per... %s ...", query);
	sprintf(message, "%s\r\n", query);
	if (send(sock_fd, message, strlen(message), 0) < 0)
	{
		perror("SEND FAILED");
	}
	
	while ((read_size=recv(sock_fd, buffer, sizeof(buffer), 0)))
	{
		*response=realloc(*response, read_size + total_size);
		if (*response==NULL)
		{
			printf("REALLOC FALLITA\n");
		}
		memcpy(*response + total_size, buffer, read_size);
		total_size+=read_size;
	}
	printf("Done\n");
	fflush(stdout);
	
	*response=realloc(*response, total_size+1);
	*(*response + total_size)='\0';
	
	close (sock_fd);
	return 0;
}

int hostname_to_ip (char *hostname, char *ip)
{
	struct hostent *he;
	struct in_addr **addr_list;
	int i;
	
	if ((he=gethostbyname(hostname))==NULL)
	{
		herror("gethostbyname");
		return 1;
	}
	
	addr_list=(struct in_addr **) he->h_addr_list;
	
	for (i=0; addr_list[i]!=NULL; i++)
	{
		strcpy (ip, inet_ntoa(*addr_list[i]));
		return 0;
	}
	return 0;
}


int main (int argc, char *argv[])
{
	char ip[100], *data=NULL;
	printf("Inserire indirizzo IP su cui fare il WhoIs\n");
	scanf("%s", ip);
	
	getWhoIs(ip, &data);
	printf("\n\n");
	puts (data);
	
	free(data);
	return 0;
}

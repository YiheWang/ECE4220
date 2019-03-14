/* 	Name       : 	client_udp.c
	Author     : 	Luis A. Rivera
	Description: 	Simple client (UDP)
					ECE4220/7220		*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_SIZE 40			// message size

void error(const char *);

int main(int argc, char *argv[])
{
   int sock, n;
   unsigned int length;
   struct sockaddr_in server, from;
   struct hostent *hp;
   char buffer[MSG_SIZE];

   if (argc != 3)
   {
	   printf("usage %s hostname port\n", argv[0]);
       exit(1);
   }

   sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
   if (sock < 0)
	   error("socket");

   server.sin_family = AF_INET;		// symbol constant for Internet domain
   hp = gethostbyname(argv[1]);		// converts hostname input (e.g. 10.3.52.15)
   if (hp == 0)
	   error("Unknown host");

   bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
   server.sin_port = htons(atoi(argv[2]));	// port field
   length = sizeof(struct sockaddr_in);		// size of structure

   printf("Please enter the message: ");
   bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used
   fgets(buffer,MSG_SIZE-1,stdin); // MSG_SIZE-1 because a null character is added

   // send message to server
   n = sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *)&server,length);
   if (n < 0)
	   error("Sendto");

   // receive from server
   n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
   if (n < 0)
	   error("recvfrom");

   printf("Got an ack: %s\n", buffer);
   close(sock);						// close socket.
   return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


/* 	Name       : 	server_tcp.c
	Author     : 	Luis A. Rivera, based on the tutorial from
					http://www.linuxhowtos.org/C_C++/socket.htm
	Description: 	Simple server (TCP)
					ECE4220/7220		*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define MSG_SIZE 40			// message size

void dostuff(int); 			// function prototype
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid, j = 0;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     sockfd = socket(AF_INET, SOCK_STREAM, 0); // Creates socket. Connection based.
     if (sockfd < 0)
    	 error("ERROR opening socket");

     // fill in fields
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);	// get port number from input
     serv_addr.sin_family = AF_INET;		 // symbol constant for Internet domain
     serv_addr.sin_addr.s_addr = INADDR_ANY; // IP address of the machine on which
											 // the server is running
     serv_addr.sin_port = htons(portno);	 // port number

     // binds the socket to the address of the host and the port number
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    	 error("ERROR on binding");

     listen(sockfd, 5);			// listen for connections
     clilen = sizeof(cli_addr);	// size of structure

	 // To allow the server to handle multiple simultaneous connections: infinite
     // loop and fork.
     while (1)
     {
    	 // blocks until a client connects to the server
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");

         j++;		// counter for the connections that are established.
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");

         if (pid == 0)	// child process
         {
        	 printf("Connection #%d created\n",j);
             close(sockfd);			// close socket
             dostuff(newsockfd);	// call function that handles communication
             exit(0);
         }
         else			// parent
         {
        	 close(newsockfd);
        	 signal(SIGCHLD,SIG_IGN);	// to avoid zombie problem
         }
     } 	// end of while

     close(sockfd);
     return 0; 		// we never get here
}

/********************************* DOSTUFF() **********************************
 There is a separate instance of this function for each connection.  It handles
 all communication once a connection has been established.
 *****************************************************************************/
void dostuff (int sock)
{
   int n;
   char buffer[MSG_SIZE];

   bzero(buffer,MSG_SIZE);
   n = read(sock,buffer,MSG_SIZE-1);	// recvfrom() could be used
   if (n < 0)
	   error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);
   n = write(sock,"I got your message",18);	// sendto() could be used.
   if (n < 0)
	   error("ERROR writing to socket");
}


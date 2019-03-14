/* 	Name       : 	client_tcp.c
	Author     : 	Luis A. Rivera, based on the tutorial from
					http://www.linuxhowtos.org/C_C++/socket.htm
	Description: 	Simple client (TCP)
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

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[MSG_SIZE];

    if (argc < 3)	// not enough arguments
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);		// port # was an input.
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Creates socket. Connection based.
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);  // converts hostname input (e.g. 10.3.52.255)
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // fill in fields of serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));	// sets all values to zero
    serv_addr.sin_family = AF_INET;		// symbol constant for Internet domain

    // copy to serv_addr.sin_addr.s_addr. Function memcpy could be used instead.
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);		// fill sin_port field

    // establish connection to the server
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("Please enter the message: ");
    bzero(buffer,MSG_SIZE);			// sets all values to zero. memset() could be used
    fgets(buffer,MSG_SIZE-1,stdin);	// MSG_SIZE-1 because a null character is added

    // send message
    n = write(sockfd,buffer,strlen(buffer));	// sendto() could be used.
    if (n < 0)
         error("ERROR writing to socket");

    bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used
    n = read(sockfd,buffer,MSG_SIZE-1);	// recvfrom() could be used
    if (n < 0)
         error("ERROR reading from socket");

    printf("%s\n",buffer);	// a message from the server
    close(sockfd);			// close socket
    return 0;
}


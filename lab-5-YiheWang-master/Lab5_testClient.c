/*
 ============================================================================
 Name        : Lab5_testClient.c
 Author      : YIhe Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

#define MSG_SIZE 40

int main(void) {
	int sock;
	int n;
	// create socket

	//send request to specific IP and port
	struct sockaddr_in serv_addr, from;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;//use IPv4 address
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//specific IP address
	//serv_addr.sin_addr.s_addr = INADDR_ANY;// get the IP of local machine
	serv_addr.sin_port = htons(1234);// port
	sock = socket(AF_INET, SOCK_DGRAM, 0);//create socket
	if(sock < 0){
		printf("Create socket error\n");
	}

	//char buffer[40];
	char buffer[40];
	int length = sizeof(struct sockaddr_in);
	while(1){
		bzero(buffer, MSG_SIZE);
		n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
	}

	printf("Message from server: %s\n",buffer);

	close(sock);

	return 0;
}

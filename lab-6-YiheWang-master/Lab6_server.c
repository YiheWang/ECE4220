/*
 ============================================================================
 Name        : Lab5_server_tcp.c
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
#include <net/if.h>
#include <arpa/inet.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MSG_SIZE 40	// message size
#define CHAR_DEV "Lab6"
int isMaster = 0;
const char space[2] = " ";
const char dot[2] = ".";
int random_number;// random number of the local machine
int ifVote = 0;

int main(int argc, const char * argv[]){
	srand(time(NULL));
	int serv_sock;
	int n;
	int boolval = 1;
	struct sockaddr_in serv_addr;//start request for specific IP and port
	struct sockaddr_in client_addr;
	struct sockaddr_in broadcast;
	socklen_t fromlen;
	char buffer[MSG_SIZE];
	char ip[13];

	if(argc != 2){
		printf("Error! Please enter port number !\n");
	}

	serv_sock = socket(AF_INET, SOCK_DGRAM, 0);//create socket
	if(serv_sock < 0){
		printf("Create socket error\n");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&client_addr, 0, sizeof(client_addr));
	serv_addr.sin_family = AF_INET;//use IPv4 address
	//serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//specific IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;// allow access to any local address
	serv_addr.sin_port = htons(atoi(argv[1]));// port

	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
		printf("Bind error!");
	}//bind the socket to the address of the host and the port number

	// set broadcast option
	if (setsockopt(serv_sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0){
	   	printf("error setting socket options\n");
	   	exit(-1);
	}
	fromlen = sizeof(struct sockaddr_in);	// size of structure

	//listen(serv_sock, 20);//waiting for client request

	//get Ip address
	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
	memcpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);// wlan0 - define the ifr_name port name
	ioctl(serv_sock, SIOCGIFADDR, &ifr);
	strcpy(ip, inet_ntoa( ( (struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	printf("Local IP address is: %s\n",ip);

	//set broadcast ip address and port
	broadcast.sin_family = AF_INET;//use IPv4 address
	broadcast.sin_addr.s_addr = inet_addr("128.206.19.255");// special broadcast ip address
	broadcast.sin_port = htons(atoi(argv[1]));// port

	while(1){
		bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used.
	   	// receive from client
       	n = recvfrom(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&client_addr, &fromlen);
       	if (n < 0){
    	  	printf("Receive error!\n");
		}

	   	printf("Received message from socket: %s\n", buffer);

		//when message equal to "WHOIS"
		if(strcmp(buffer,"WHOIS\n") == 0){
			printf("WHOIS start!\n");
			bzero(buffer,MSG_SIZE);
			if(isMaster == 1){
				sprintf(buffer,"Yihe Wang is master %s",ip);
				n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&client_addr, fromlen);
				if(n < 0){
					printf("Error sending!\n");
				}
				// send message to
			}
			printf("\n");
		}

		//when message equal to "VOTE"
		else if(strcmp(buffer,"VOTE\n") == 0){
			printf("VOTE start!\n");
			bzero(buffer,MSG_SIZE);
			random_number = 1 + rand() % 10;//generate random number

			sprintf(buffer,"# %s %d",ip,random_number);
			//copy message into buffer
			printf("Send message %s\n",buffer);

			n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&broadcast, fromlen);
			if(n < 0){
				printf("Sending error!\n");
			}
			ifVote = 1;//a new term of game start 
			printf("\n");
		}

		//when the first character equal to '@'
		else if(buffer[0] == '@' && buffer[2] != '1'){
			printf("Case @ start !\n");
			int cdev_id;
			int dummy;
			char temp_buffer[MSG_SIZE];
			switch(buffer[1]){
				case 'A':
					sprintf(temp_buffer,"@A");
					break;
				case 'B':
					sprintf(temp_buffer,"@B");
					break;
				case 'C':
					sprintf(temp_buffer,"@C");
					break;
				case 'D':
					sprintf(temp_buffer,"@D");
					break;
				case 'E':
					sprintf(temp_buffer,"@E");
					break;
				}
			if((cdev_id = open("/dev/Lab6", O_WRONLY)) == -1) {
				printf("Cannot open device %s\n", CHAR_DEV);
				exit(1);
			}
			dummy = write(cdev_id, temp_buffer, sizeof(temp_buffer));
				if(dummy != sizeof(temp_buffer)) {
					printf("Write failed, leaving...\n");
				break;
			}
			// server is master
			if(isMaster){
				switch(buffer[1]){
					case 'A':
						sprintf(buffer,"@A1");
						printf("Send message %s\n",buffer);
						n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&broadcast, fromlen);
							if(n < 0){
							printf("Sending error!\n");
						}
						break;
					case 'B':
						sprintf(buffer,"@B1");
						printf("Send message %s\n",buffer);
						n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&broadcast, fromlen);
							if(n < 0){
							printf("Sending error!\n");
						}
						break;
					case 'C':
						sprintf(buffer,"@C1");
						printf("Send message %s\n",buffer);
						n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&broadcast, fromlen);
							if(n < 0){
							printf("Sending error!\n");
						}
						break;
					case 'D':
						sprintf(buffer,"@D1");
						printf("Send message %s\n",buffer);
						n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&broadcast, fromlen);
							if(n < 0){
							printf("Sending error!\n");
						}
						break;
					case 'F':
						sprintf(buffer,"@F1");
						printf("Send message %s\n",buffer);
						n = sendto(serv_sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&broadcast, fromlen);
							if(n < 0){
							printf("Sending error!\n");
						}
						break;
					}
				}
		}
		//when the first character equal to '#'
		else if(buffer[0] == '#'){
			printf("Judge who is master start!\n");
			//isMaster = 1;// if I am the only guy, I am master
			if(ifVote == 0){
				printf("The new game did not start!\n");
				break;// a new game is not start yet
			}
			char *token;
			char *raw_ip;
			char *raw_num;
			token = strtok(buffer,space);
			raw_ip = strtok(NULL,space);//raw ip contain '.' point
			raw_num = strtok(NULL,space);
			int vote_num = atoi(raw_num);

			char *char_ip_other = "";
			char_ip_other = strtok(raw_ip,dot);
			int i;
			for(i=0;i<3; ++i){
				char *temp;
				temp = strtok(NULL,dot);
				sprintf(char_ip_other,"%s%s",char_ip_other,temp);
			}
			int int_ip_other = atoi(char_ip_other);//integer ip
			//get ip number in integer, remove '.' in ip address

			if(random_number > vote_num){
				isMaster = 1; // I am the master
				//printf("%d\n",random_number)
			}
			else if(random_number == vote_num){
				char *tempIP;
				strcpy(tempIP,ip);
				char *char_ip_mine;
				char_ip_mine = strtok(tempIP,dot);
				int i;
				for(i=0;i<3; ++i){
					char *temp;
					temp = strtok(NULL,dot);
					sprintf(char_ip_mine,"%s%s",char_ip_mine,temp);
				}
				int int_ip_mine = atoi(char_ip_mine);//integer ip
				if(int_ip_mine < int_ip_other){
					isMaster = 0;//I am not master
				}
				else if(int_ip_mine == int_ip_other){
					isMaster = 1;//I am the only guy, I am master
				}
			}
			else if(random_number < vote_num){
				isMaster = 0;
			}
			ifVote = 0;//This term is end, status cannot be chanegd
			printf("\n");
		}

	}


	// accept the request from client
	//socklen_t client_addr_size  = sizeof(client_addr);
	//client_sock = accept(serv_sock, (struct sockaddr*)&client_addr, &client_addr_size);

	//send message to client
	//char message[MSG_SIZE] = "Hello World";
	/*char *message = "Hello World";
	while(1){
		bzero(buffer,MSG_SIZE);
		n = sendto(serv_sock, message, MSG_SIZE,0,(struct sockaddr *)&client_addr,fromlen);
	}*/
	//write(client_sock,message, sizeof(message));


	return 0;
}















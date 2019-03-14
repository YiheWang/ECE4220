/*
 ============================================================================
 Name        : Lab4_2.c
 Author      : YIhe Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <semaphore.h>
#include <wiringPi.h>
#include <sys/types.h>
#include "ece4220lab3.h"

#define Button1 27
#define MY_PRIORITY 51

int main(void) {
	wiringPiSetup();
	//pinMode(Button1,INPUT);
	pullUpDnControl(Button1,PUD_DOWN);
	
	
	int timer_fd = timerfd_create(CLOCK_MONOTONIC,0);
	struct itimerspec itval;
	//structure to hold period and starting time

	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 60000000;
	//timer fires every 60ms

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 1;
	//timer will start in

	timerfd_settime(timer_fd, 0, &itval, NULL);
	//start the timer

	//initialize current thread as RT task
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	// create pipe
	int N_pipe2;
	char *pipePath = "/tmp/myfifo";
	if(!access(pipePath,F_OK)){
		unlink(pipePath);
	}//delete pipe if existed
	//mkfifo(filePath,0666);
	int command = system("mkfifo /tmp/N_pipe2");
	printf("hello\n");
	if((N_pipe2 = open("/tmp/N_pipe2", O_WRONLY)) < 0){
		printf("pipe created error!\n");
		exit(1);
	}

	printf("hahaha\n");
	// write time stamp into pipe
	struct timeval current_time;
	uint64_t num_periods = 0;
	/*read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1) {
			puts("MISSED WINDOW\n");
			exit(1);
		}*/

	while(1){
		if(check_button()){
			gettimeofday(&current_time,NULL);//get current time
			printf("Hello\n");
			
			if(write(N_pipe2,&current_time,sizeof(current_time)) < 0){
				printf("pipe write error!\n");
				return 0;
			}
			printf("Hello\n");
			clear_button();
		}

		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1) {
			puts("MISSED WINDOW\n");
			exit(1);
		}
	}

	return 0;


}




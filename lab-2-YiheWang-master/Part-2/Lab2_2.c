/*
 ============================================================================
 Name        : Lab2_2.c
 Author      : YIhe Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <pthread.h>

#define MY_PRIORITY 51  // kernel is priority 50
#define MAX_LINE 1024

char buffer[MAX_LINE];
FILE *file1;
FILE *file2;
FILE *file3;
int count = 0;


void ThreadFirstLine(void *ptr){

	//configure timer
	int timer_fd = timerfd_create(CLOCK_MONOTONIC,0);

	struct itimerspec itval;
	//structure to hold period and starting time

	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 20000000;
	//timer fires every 20ms

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 5000000;
	//timer will start in 5ms

	timerfd_settime(timer_fd, 0, &itval, NULL);
	//start the timer

	//initialize current thread as RT task
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;
	while(!feof(file1)){
		fgets(buffer,MAX_LINE,file1);
		printf("%s",buffer);
		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1) {
			puts("MISSED WINDOW");
			exit(1);
		}
	}

	pthread_exit(0);
}

void ThreadSecondLine(void *ptr){
	//configure timer
	int timer_fd = timerfd_create(CLOCK_MONOTONIC,0);

	struct itimerspec itval;
	//structure to hold period and starting time

	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 20000000;
	//timer fires every 20ms

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 15000000;
	//timer will start in 15ms

	timerfd_settime(timer_fd, 0, &itval, NULL);
	//start the timer

	//initialize current thread as RT task
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;
	while(!feof(file2)){
		fgets(buffer,MAX_LINE,file2);
		printf("%s",buffer);
		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1) {
			puts("MISSED WINDOW");
			exit(1);
		}
	}

	pthread_exit(0);
}

void ThreadWriteInFile(void *ptr){
	//configure timer
	int timer_fd = timerfd_create(CLOCK_MONOTONIC,0);

	struct itimerspec itval;
	//structure to hold period and starting time

	itval.it_interval.tv_sec = 0;
	itval.it_interval.tv_nsec = 10000000;
	//timer fires every 10ms

	itval.it_value.tv_sec = 0;
	itval.it_value.tv_nsec = 10000000;
	//timer will start in 10ms

	timerfd_settime(timer_fd, 0, &itval, NULL);
	//start the timer

	//initialize current thread as RT task
	struct sched_param param;
	param.sched_priority = MY_PRIORITY;
	sched_setscheduler(0, SCHED_FIFO, &param);

	uint64_t num_periods = 0;
	int i;
	for(i=0;i<count;++i){
		//printf("%s\n",buffer);
		if(i == (count-1)){
			fprintf(file3,"%s\n",buffer);
		}
		fprintf(file3,"%s",buffer);
		read(timer_fd, &num_periods, sizeof(num_periods));
		if(num_periods > 1) {
			puts("MISSED WINDOW");
			exit(1);
		}
	}

	pthread_exit(0);
}

void CountLine(FILE *file, char line[50])
{
	int flag = 0;
	file = fopen(line,"r");
	while(!feof(file)){
		flag = fgetc(file);
		if(flag == '\n')
		++count;
	}
	++count;
	fclose(file);
}


int main() {
	char fileName1[50] = "first.txt";
	char fileName2[50] = "second.txt";
	char fileName3[50] = "merged.txt";
	CountLine(file1,fileName1);
	CountLine(file2,fileName2);

	printf("Line number: %d\n",count);


	pthread_t threadFirstLine;
	pthread_t threadSecondLine;
	pthread_t threadWriteInFile;

	file1 = fopen(fileName1,"r");
	file2 = fopen(fileName2,"r");
	file3 = fopen(fileName3,"w");//open files

	pthread_create(&threadFirstLine,NULL,(void *)&ThreadFirstLine,NULL);
	//read first file
	pthread_create(&threadSecondLine,NULL,(void *)&ThreadSecondLine,NULL);
	//read second file
	pthread_create(&threadWriteInFile,NULL,(void *)&ThreadWriteInFile,NULL);
	//write in third file

	pthread_join(threadFirstLine,NULL);
	pthread_join(threadSecondLine,NULL);
	pthread_join(threadWriteInFile,NULL);

	fclose(file1);
	fclose(file2);
	fclose(file3);

	return 0;
}

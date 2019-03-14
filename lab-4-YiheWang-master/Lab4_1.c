/*
 ============================================================================
 Name        : Lab4_1.c
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
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <time.h>
#include <semaphore.h>
#include <wiringPi.h>
#include <sys/types.h>

typedef struct GPS{
	struct timeval GPS_time;
	char buffer;
}GPS;
typedef struct TimeStamp{
	int buffer1;
	struct timeval GPS_last;
	int buffer2;
	struct timeval GPS_next;
	double interval;
	struct timeval pressButton;
}TimeStamp;

GPS GPS_event;
TimeStamp timeStamp;
sem_t sem;

void Kid_Thread(void *Ptr){
	timeStamp.buffer1 = (int)GPS_event.buffer;
	timeStamp.GPS_last = GPS_event.GPS_time;
	//struct timeval *bP = (struct timeval *)Ptr;
	timeStamp.pressButton = *((struct timeval *)Ptr);
	while(1){
		if(timeStamp.buffer1 != (int)GPS_event.buffer){
			timeStamp.buffer2 = (int)GPS_event.buffer;
			timeStamp.GPS_next = GPS_event.GPS_time;
			break;
		}
	}// waiting for next GPS signal
	double timeTemp[3];
	timeTemp[0] = (double)timeStamp.pressButton.tv_sec * 1000000 + (double)timeStamp.pressButton.tv_usec;
	timeTemp[1] = (double)timeStamp.GPS_last.tv_sec * 1000000 + (double)timeStamp.GPS_last.tv_usec;
	timeTemp[2] = timeTemp[1] - timeTemp[0];
	timeStamp.interval = (double)timeStamp.buffer1 + (double)timeTemp[2]/250000;
	sem_wait(&sem);//wait for semophore
	printf("Child Thread:\n");
	printf("%d, last GPS: second:%ld, microsecond:%ld\n",timeStamp.buffer1,timeStamp.GPS_last.tv_sec,timeStamp.GPS_last.tv_usec);
	printf("%d, next GPS: second:%ld, microsecond:%ld\n",timeStamp.buffer2,timeStamp.GPS_next.tv_sec,timeStamp.GPS_next.tv_usec);
	printf("Button press time: second:%ld, microsecond:%ld\n",timeStamp.pressButton.tv_sec,timeStamp.pressButton.tv_usec);
	printf("Interval for button press is: %lf\n\n",timeStamp.interval);
	sem_post(&sem);//release the semophore
}

void Read_button(){
	int N_pipe2;
	struct timeval pressButton;
	pthread_t kid_thread[5];
	int i = 0;
	if((N_pipe2 = open("/tmp/N_pipe2",O_RDONLY)) <= 0){
		printf("Pipe open error"); // Open FIFO
		exit(0);
	}
	while(1){
		if(read(N_pipe2,&pressButton, sizeof(pressButton)) < 0){
			printf("Pipe read error");
			exit(0);
		}
		else{
			pthread_create(&kid_thread[i],NULL,(void *)&Kid_Thread,(void *)&pressButton);
			//printf("%ld.%ld\n",pressButton.tv_sec,pressButton.tv_usec);// should print something different
			if(i == 4){
				i = 0;
			}
			++i;
		}
	}
	close(N_pipe2);
	pthread_exit(0);
}

int main(void) {
	int N_pipe1;
	int int_buffer;
	pthread_t read_button;
	sem_init(&sem,0,1);
	if((N_pipe1 = open("/tmp/N_pipe1",O_RDONLY)) < 0){
		printf("Pipe open error");// Open FIFO
		return 0;
	}
	
	GPS_event.buffer = '1';
	GPS_event.GPS_time.tv_sec = 0;
	GPS_event.GPS_time.tv_usec = 0;

	struct timeval pressButton;
	pthread_create(&read_button,NULL,(void *)&Read_button,NULL);
	//pthread_join(read_button,NULL);

	//Read FIFO
	while(1){
		//usleep(250000);//250ms
		if(read(N_pipe1,&GPS_event.buffer, sizeof(GPS_event.buffer)) < 0){
			printf("Pipe read error");
			return 0;
		}
		gettimeofday(&GPS_event.GPS_time,NULL);
		//int_buffer = (int)GPS_event.buffer;
		//printf("%d, %ld.%ld\n",int_buffer,GPS_event.GPS_time.tv_sec,GPS_event.GPS_time.tv_usec);
	}

	close(N_pipe1);

	//Close FIFO

	return 0;
}

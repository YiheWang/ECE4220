/*
 ============================================================================
 Name        : Lab3_2.c
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
#include "ece4220lab3.h"

int priorityRed = 55;
int priorityGreen = 60;
int priorityBlue = 65;
sem_t sem;

#define LEDRed 8
#define LEDGreen 7
#define LEDBlue 21
#define LEDYellow 9
#define Button1 27

void ThreadRed()
{
	struct sched_param param;
	param.sched_priority = priorityRed;
	sched_setscheduler(0,SCHED_FIFO,&param);//set the priority of red LED

	while(1){
		sem_wait(&sem);
		//wait for the turn of this thread
		digitalWrite(LEDRed,HIGH);
		sleep(1);
		digitalWrite(LEDRed,LOW);
		sem_post(&sem);
		usleep(50);
		//release the semaphore
	}

	pthread_exit(0);
}

void ThreadGreen()
{
	struct sched_param param;
	param.sched_priority = priorityGreen;
	sched_setscheduler(0,SCHED_FIFO,&param);//set the priority of Green LED

	while(1){
		sem_wait(&sem);
		//wait for the turn of this thread
		digitalWrite(LEDGreen,HIGH);
		sleep(1);
		digitalWrite(LEDGreen,LOW);
		sem_post(&sem);
		usleep(50);
		//release the semaphore
	}

	pthread_exit(0);
}

void ThreadBlue()
{
	struct sched_param param;
	param.sched_priority = priorityBlue;
	sched_setscheduler(0,SCHED_FIFO,&param);//set the priority of Blue LED

	while(1){
		sem_wait(&sem);
		if(check_button()){

			//wait for the turn of this thread
			digitalWrite(LEDBlue,HIGH);
			sleep(1);
			digitalWrite(LEDBlue,LOW);

			clear_button();//clear the button signal
		}
		//release the semaphore
		sem_post(&sem);
		usleep(50);
	}
	pthread_exit(0);
}


int main(void) {
	wiringPiSetup();//set up pi

	pthread_t threadRed;//red LED represents one direction
	pthread_t threadGreen;//Green LED represents another direction
	pthread_t threadBlue;//Blue LED represents pedestrian light

	pinMode(LEDRed,OUTPUT);
	pinMode(LEDGreen,OUTPUT);
	pinMode(LEDBlue,OUTPUT);//set pin mode on
	pinMode(LEDYellow,OUTPUT);

	digitalWrite(LEDRed,LOW);
	digitalWrite(LEDYellow,LOW);
	digitalWrite(LEDBlue,LOW);
	digitalWrite(LEDGreen,LOW);//low all the LED first
	pullUpDnControl(Button1, PUD_DOWN);

	sem_init(&sem,0,1);
	//create the semaphore, only one thread are able to run in one period of time

	//printf("Please enter the priority number(between 51-99) of LED Red,Green and Blue: ");
	//scanf("%d %d %d ",&priorityRed,&priorityGreen,&priorityBlue);

	pthread_create(&threadRed,NULL,(void *)&ThreadRed,NULL);
	pthread_create(&threadGreen,NULL,(void *)&ThreadGreen,NULL);
	pthread_create(&threadBlue,NULL,(void *)&ThreadBlue,NULL);

	pthread_join(threadRed,NULL);
	pthread_join(threadGreen,NULL);
	pthread_join(threadBlue,NULL);


	return 0;
}

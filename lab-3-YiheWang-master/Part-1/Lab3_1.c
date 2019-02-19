/*
 ============================================================================
 Name        : Lab3_1.c
 Author      : YIhe Wang
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>
#include "ece4220lab3.h"

#define LEDRed 8
#define LEDGreen 7
#define LEDBlue 21
#define LEDYellow 9
#define Button1 27

void ThreadLight(void)
{
	int flag;

	while(1){
		flag = check_button();
		if(flag){

			digitalWrite(LEDBlue,HIGH);
			usleep(500000);
			digitalWrite(LEDBlue,LOW);
			clear_button();//clear the button signal
			continue;
		}//LED for pedestrians

		//printf("hahaha\n");

		digitalWrite(LEDRed,HIGH);
		usleep(500000);
		digitalWrite(LEDRed,LOW);//LED for one direction

		digitalWrite(LEDGreen,HIGH);
		usleep(500000);
		digitalWrite(LEDGreen,LOW);//LED for another direction
	}

	pthread_exit(0);
}

int main(void) {
	wiringPiSetup();
	pthread_t threadLight;

	pinMode(LEDRed,OUTPUT);
	pinMode(LEDGreen,OUTPUT);
	pinMode(LEDBlue,OUTPUT);//set pin mode on
	pinMode(LEDYellow,OUTPUT);

	digitalWrite(LEDRed,LOW);
	digitalWrite(LEDYellow,LOW);
	digitalWrite(LEDBlue,LOW);
	digitalWrite(LEDGreen,LOW);


	pthread_create(&threadLight,NULL,(void *)&ThreadLight,NULL);
	pthread_join(threadLight,NULL);

	return 0;
}

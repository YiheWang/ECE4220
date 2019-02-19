#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>

#define LEDRed 8
#define LEDYellow 9
#define LEDGreen 7
#define LEDBlue 21
#define Button1 27
#define Button2 0
#define Button3 1
#define Button4 24
#define Button5 28
#define Buzzer 22


int main(int argc, char **argv) {

	wiringPiSetup();
	//wiringPiSetupGpio();


	pinMode(LEDRed,OUTPUT);
	pinMode(LEDYellow, OUTPUT);
	pinMode(LEDGreen, OUTPUT);
	pinMode(LEDBlue, OUTPUT);

	int i;
	for(i=0; i<10;++i){
		//digitalWrite(LEDRed,HIGH);
		digitalWrite(LEDRed,HIGH);
		sleep(1);
		digitalWrite(LEDRed,LOW);
		digitalWrite(LEDYellow,HIGH);
		sleep(1);
		digitalWrite(LEDYellow,LOW);
		digitalWrite(LEDGreen,HIGH);
		sleep(1);
		digitalWrite(LEDGreen,LOW);
		digitalWrite(LEDBlue,HIGH);
		sleep(1);
		digitalWrite(LEDBlue,LOW);
	}//use for loop to light each LED 10 times




	int enterNum;
	printf("Please enter a number between 1-5:");
	scanf("%d",&enterNum);

	while(enterNum<0||enterNum>5){
		printf("Enter again, range of number is not correct!\n");
		printf("Please enter a number between 1-5:");
		scanf("%d",&enterNum);
	}// error check

	switch(enterNum){
		case 1:{
			while(1){
				//run infinitely until press button 1
				if(digitalRead(Button1)){
					int i;
					for(i=0;i<5000;++i){
						pullUpDnControl(Buzzer,PUD_UP);
						usleep(500);
						pullUpDnControl(Buzzer,PUD_DOWN);
						usleep(500);
					}
					break;
				}
			}
			break;
		}// case 1, press button 1
		case 2:{
			while(1){
				//run infinitely until press button 2
				if(digitalRead(Button2)){
					int i;
					for(i=0;i<5000;++i){
						pullUpDnControl(Buzzer,PUD_UP);
						usleep(500);
						pullUpDnControl(Buzzer,PUD_DOWN);
						usleep(500);
					}
					break;
				}
			}
			break;
		}
		case 3:{
			while(1){
				//run infinitely until press button 2
				if(digitalRead(Button3)){
					int i;
					for(i=0;i<5000;++i){
						pullUpDnControl(Buzzer,PUD_UP);
						usleep(500);
						pullUpDnControl(Buzzer,PUD_DOWN);
						usleep(500);
					}
					break;
				}
			}
			break;
		}
		case 4:{
			while(1){
				//run infinitely until press button 2
				if(digitalRead(Button4)){
					int i;
					for(i=0;i<5000;++i){
						pullUpDnControl(Buzzer,PUD_UP);
						usleep(500);
						pullUpDnControl(Buzzer,PUD_DOWN);
						usleep(500);
					}
					break;
				}
			}
			break;
		}
		case 5:{
			while(1){
				//run infinitely until press button 2
				if(digitalRead(Button5)){
					int i;
					for(i=0;i<5000;++i){
						pullUpDnControl(Buzzer,PUD_UP);
						usleep(500);
						pullUpDnControl(Buzzer,PUD_DOWN);
						usleep(500);
					}
					break;
				}
			}
			break;
		}
	}

	return 0;
}


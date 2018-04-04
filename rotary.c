/* 
 *  rotary Control
 *  Author       : sana2dang ( fly9p ) - sana2dang@naver.com
 *  Cafe         : http://cafe.naver.com/raspigamer
 *  Thanks to    : zzeromin, smyani, GreatKStar, KimPanda, StarNDevil, angel
 * 
 * - Reference -
 * wiringPi
 * http://wiringpi.com/the-gpio-utility/
 * apt-get install wiringPi
 * 
 * - complie -
 * sudo gcc rotary.c -o rotary -lwiringPi
 * 
 */

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>

#define RoAPin 8
#define RoBPin 11

#define controlPinA 23
#define controlPinB 14



unsigned char Last_RoB_Status;
unsigned char Current_RoB_Status;

time_t sTime	= 0;	// AbtnPress Start Time
time_t eTime	= 0;	// AbtnPress End Time

int A = 0;
int B = 0;

int A_temp = 0;
int B_temp = 0;

int A_Flag = 0;
int B_Flag = 0;


int debug = 0;
int initControl  = 0;


void control()
{	
	Last_RoB_Status = digitalRead(RoBPin);	

  	Current_RoB_Status = digitalRead(RoAPin);


	/*
	if( debug == 1 )
		printf("A %d B : %d\n", Last_RoB_Status, Current_RoB_Status );
	*/
	

	
	if( Last_RoB_Status != A && B_Flag != 1 )	// Right
	{
		A_Flag = 1;
		A_temp = Last_RoB_Status;
		if( Current_RoB_Status != B )
		{
			B_temp = Current_RoB_Status;
			
			if( debug == 1 )
				printf("Right Init : %d / %d %d\n", initControl, Last_RoB_Status, Current_RoB_Status );

			pullUpDnControl(controlPinB, PUD_UP);
			pullUpDnControl(controlPinA, PUD_DOWN);	
			sTime = millis();
			A = A_temp;
			B = B_temp;
			A_Flag = 0;
			initControl = 0;
		}
	}	
	else if( Current_RoB_Status != B && A_Flag != 1 )	// Left
	{
		B_Flag = 1;
		B_temp = Current_RoB_Status;
		if( Last_RoB_Status != A )
		{
			A_temp = Last_RoB_Status;

			if( debug == 1 )						
				printf("Left Init : %d / %d %d\n", initControl, Last_RoB_Status, Current_RoB_Status );

			pullUpDnControl(controlPinA, PUD_UP);
			pullUpDnControl(controlPinB, PUD_DOWN);
			sTime = millis();

			A = A_temp;
			B = B_temp;
			B_Flag = 0;
			initControl = 0;

		}
	}
	
	//Last_RoB_Status = 0;
	//Current_RoB_Status = 0;

}


int main (int argc, char **argv)
{
	int cutTime = 120;

	if (argc == 2)
	{	
		if( atoi( argv[1] ) == 0 )
		{	
			debug = 1;	
			cutTime = 120;
		}
		else
		{
			cutTime = atoi( argv[1] );
		}
		
		
	}	
	else
	{
		cutTime = 120;
	}

	if( wiringPiSetupGpio() == -1 )
		return 0;

	pinMode(RoAPin, INPUT);
	pinMode(RoBPin, INPUT);
	pullUpDnControl(RoAPin, PUD_UP);
	pullUpDnControl(RoBPin, PUD_UP);

	while(1)
	{	
		control();
		eTime = millis();
		if( eTime - sTime > cutTime)
		{	
			if( initControl == 0 )
			{
				//printf("L R Pull UP\n");
				pullUpDnControl(controlPinA, PUD_UP);
				pullUpDnControl(controlPinB, PUD_UP);	
				initControl = 1;
			}
		}		
		usleep(1000);
	}



	return -1;
}

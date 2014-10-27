#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <getopt.h>
#include <cstdlib>
#include <iostream>
#include "/home/pi/RF24/RPi/RF24/RF24.h"
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

using namespace std;

#define DEBUG 1
#define NUM_THREADS 20
#define CMD_LINE_MAX 100

//RF24 radio("/dev/spidev0.0",8000000 , 25);  //spi device, speed and CSN,only CSN is NEEDED in RPI
RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_1MHZ);
const int role_pin = 7;
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

struct cmdToThread{
    char cmd[100];
    int currentThreadId;
};

void setup(void){
	//Prepare the radio module
	printf("\nPreparing interface\n");
	radio.begin();
	radio.setChannel(0x4c);
	radio.setPALevel(RF24_PA_MAX);

        radio.setDataRate(RF24_250KBPS);
        radio.setAutoAck(false);
	radio.setRetries( 1, 1);

	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);

	radio.startListening();
	radio.printDetails();
        printf("\nFinished interface\nRadio Data Available:%s", radio.available()?"Yes":"No");
}

/*
bool switchLight(int action){
	//This function send a message, the 'action', to the arduino and wait for answer
	//Returns true if ACK package is received
	//Stop listening
	radio.stopListening();
	unsigned long message = action;
	printf("Now sending %lu...", message);
	
	//Send the message
	bool ok = radio.write( &message, sizeof(unsigned long) );
	if (ok)
		printf("ok...");
	else
		printf("failed.\n\r");
	//Listen for ACK
	radio.startListening();
	//Let's take the time while we listen
	unsigned long started_waiting_at = millis();
	bool timeout = false;
	while ( ! radio.available() && ! timeout ) {
		delayMicroseconds(10);
		if (millis() - started_waiting_at > 1000 )
			timeout = true;

	}

	if( timeout ){
		//If we waited too long the transmission failed
			printf("Failed, response timed out.\n\r");
			return false;
	}else{
		//If we received the message in time, let's read it and print it
		unsigned long got_time;
		radio.read( &got_time, sizeof(unsigned long) );
		printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
		return true;
	}
	 
}  
*/

void getAndPrintIPAdr()
{
 int fd;
 struct ifreq ifr;

 fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 close(fd);

 /* display result */
 printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}


void* sendDataToCloud(void *cmd)
{
    struct cmdToThread sNew;

    memcpy(&sNew, cmd, sizeof(cmdToThread));

    getAndPrintIPAdr(); 
    printf("Command in the Thread:%s\r\n", sNew.cmd);

    system(sNew.cmd);

    //printf("execution status: %i\r\n", i);

    printf("\n\nExiting the Thread Nr.%i\n\n", sNew.currentThreadId);

    return NULL;
}

int main( int argc, char ** argv){

        //char choice;
	setup();
	//bool switched = false;
	//int counter = 0;
        pthread_t threads[NUM_THREADS];
        int nextThreadId = 0;

//Define the options

        radio.startListening();
        //Let's take the time while we listen
        //unsigned long started_waiting_at = millis();
        //bool timeout = false;
//        while ( ! radio.available() && ! timeout ) {
        while(1){
            delayMicroseconds(40000);

            if(radio.available())
            {
                //printf("Radio Data Available!!!\n");
                char abc[100];
                radio.read( abc, sizeof(abc) );

                struct cmdToThread s; 

                //printf("Got response %s, round-trip delay: \n\r", abc);                
                snprintf(s.cmd, sizeof(s.cmd), "/home/pi/IoT_Acc/cgateway/gw.py %s", abc);
             
                if (nextThreadId >= NUM_THREADS)
                  nextThreadId = 0;

                s.currentThreadId = nextThreadId;

                int err = pthread_create(&(threads[nextThreadId]), NULL, &sendDataToCloud, (void*)&s);
                pthread_detach(threads[nextThreadId]);

                if (err != 0)
                  printf("\nCan't create thread :[%s]", strerror(err));
                else
                  printf("\n Thread Nr.%i created!\n", s.currentThreadId);

                nextThreadId++;


                //unsigned long got_time;
                //radio.read( &got_time, sizeof(unsigned long) );
                //printf("Got response %lu, round-trip delay: \n\r",got_time);

                //delayMicroseconds(20*10);
                //radio.stopListening();

/*                
                //Send the message
                bool ok = radio.write( &(got_time), sizeof(unsigned long) );
                if (ok)
                    printf("ok...");
                else
                    printf("failed.\n\r");
                    //Listen for ACK
*/
                radio.startListening();
                delayMicroseconds(20*10);
            }
        }
}

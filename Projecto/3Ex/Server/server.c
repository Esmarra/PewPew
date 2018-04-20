/*=========================================
Program Name :	server.c
Base Language:	Mixed
Created by   :	Esmarra
Creation Date:	16/04/2018
Rework date entries:

Program Objectives:
  * Recieve (1 by 1) a bin in float from client
  * Convert to ASCII write to .ASC

Observations:
Special Thanks:
=========================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
//==== DEFAULT SOCKETS ====//
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
//==== FILE I/O ====//
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h> //unit64 <-- remove?
#define MANTISSA 50 // Bits Res in conv to bin

char bin[MANTISSA];
float bin2fp(char* digits);

int main(int argc, char **argv){
  float buf,flt_max; // Receive over socket
  int sock_s,sock_c;
  struct sockaddr_un server;
  memset(&server, 0, sizeof(server));

  sock_s= socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock_s < 0){
    perror("Creating Server Socket");
    exit(0);
  }
  server.sun_family = AF_UNIX; // Address
  //strcpy(server.sun_path, "/tmp"); // Bind socket to File
  strncpy(server.sun_path, "/tmp/socket", sizeof(server.sun_path)-1);
  //unlink("/tmp"); // Delete server file
  //if(bind(sock_s, (struct sockaddr *) &server, SUN_LEN(&server)) < 0){
  FILE *ficheiro1;
  ficheiro1 = fopen("output.asc","w");
  //int fd;
  //fd = open("output.asc", O_WRONLY|O_CREAT|O_TRUNC);
  //const void *buff;

  unlink("/tmp/socket"); // Unlink socket
  if(bind(sock_s, (struct sockaddr*)&server, sizeof(server))<0){
    perror("Starting Server Socket");
    exit(0);
  }
  printf(">Server is running\n" );
  //==== Server Is Listening ====//
  if(listen(sock_s, 8) < 0){ // Max connections
    perror("Listening Server Socket");
    exit(0);
  }
  //==== xxxxxxxxxxxxxxxxxxx ====//

  //==== Acept Client ====//
  sock_c = accept(sock_s, NULL, NULL);
  if(sock_c < 0){
    perror("Accepting Client Socket");
    exit(0);
  }

  do{
    //==== Receive Data ====//
    if(recv(sock_c, &buf, sizeof(buf), 0) < 0){
      perror("Receiving Data From Client");
      exit(0);
    }

    if(buf!=-1){ // Data is valid
      if(buf==-2){ // -2 <-- FLT_MAX is next
        if(recv(sock_c, &buf, sizeof(buf), 0) < 0){ //get
          perror("Receiving Data From Client");
          exit(0);
        }
        flt_max=buf;
        printf("FLT_MAX=%1.0f\n",flt_max); //Disp
        break;
        //exit(0);// FINISH
      }

      printf("Rcv: %.10f\n",buf );
      snprintf(bin, sizeof bin, "%.10f", buf); // Copy float(buf) to char(bin)
      //write(1,&bin,sizeof bin);
      buf=bin2fp(bin); // Conv 2 flt
      //printf("Float: %f\n",buf);
      fprintf(ficheiro1,"%f\n",buf); //write to file
    }

  }while(buf != -1);

  close(sock_s); // Close Server
	close(sock_c); // Close Client
  unlink("/tmp/socket"); // Cleanup
  printf("THE END\n" );
  exit(0);
}

float bin2fp(char* digits){
    int befDec[MANTISSA], aftDec[MANTISSA];
    int aftDecDigits, befDecDigits, storeIntegral=0, i, j=0, k=0;
    float storeFractional=0, floatValue;
    char up = 'd';
		//printf("digits=%s\n",digits);
    for(i=0; i<strlen(digits); i++){  // Separar int das frac
        if(digits[i]=='.'){
            up='u';
        }
        else if(up=='d'){
            befDec[i] = (int)digits[i]-48;
            k++;
        }
        else{
            aftDec[j] = (int)digits[i]-48;
            j++;
        }
    }
    befDecDigits = k;
    aftDecDigits = j;

    j=0;
    for(i = befDecDigits-1; i>=0; i--){
        storeIntegral = storeIntegral + (befDec[i] *(int) pow(2,j));
        j++;
    }

    j = -1;
    for(i = 0; i<aftDecDigits; i++){
        storeFractional = storeFractional + (aftDec[i]*pow(2,j));
        j--;
    }

    floatValue = storeIntegral + storeFractional;
		return(floatValue);
}

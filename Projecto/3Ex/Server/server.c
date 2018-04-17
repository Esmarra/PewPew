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
//==== DEFAULT SOCKETS ====//
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


int main(){
  float buf; // Receive over socket
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

    if(buf!=-1){
      //conv data from bin to float to ASCII
      // add 0 se 9<data<100
      // add 00 se 0<data<10
      if(buf==-2){
        if(recv(sock_c, &buf, sizeof(buf), 0) < 0){
          perror("Receiving Data From Client");
          exit(0);
        }
        printf("FL=%f\n",buf);
        exit(0);
      }
      fprintf(ficheiro1,"%f ",buf);
      printf("Rcv: %f\n",buf );
    }

  }while(buf != -1);

  close(sock_s); // Close Server
	close(sock_c); // Close Client
  unlink("/tmp/socket"); // Cleanup
}

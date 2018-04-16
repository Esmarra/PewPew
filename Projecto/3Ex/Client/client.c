/*=========================================
Program Name :	client.c
Base Language:	Mixed
Created by   :	Esmarra
Creation Date:	14/04/2018
Rework date entries:

Program Objectives:
  * Open .asc and .bin
  * Read 1 from .asc Write 1 to .bin
  * Close .asc .bind
  * Open .bin read to float array (one pass) -> mmap() ?
  * Send floats to server (1 by one)
  * End by sendind FLT_MAX (checksum)
Observations:
  * TxT to ASCII : http://www.unit-conversion.info/texttools/ascii/
Special Thanks:
=========================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <errno.h>   /* errno */
//==== DEFAULT SOCKETS ====//
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
//========================//
#define FLT_MAX 20 // Max Lines in ASC File
#define MANTISSA 23

char read_file_name[]="sauce/input.asc";   // Read asc file located at /sauce/
char write_file_name[]="sauce/input.bin";   // Write bin file located at /sauce/
// Conv to Binary
void fp2bin_i(double fp_int, char* binString);
void fp2bin_f(double fp_frac, char* binString);
void fp2bin(double fp, char* binString);
char bin[MANTISSA];


struct Data{
  float number[FLT_MAX];
  int num; // Current number in order <-- USELESS
};

int main(){ // client
  int sock;
  struct sockaddr_un server;

  struct Data data; //Start Events struct
  data.num=1; //Init zero XXXXXXXXXXXXXXX ZERO OU 1?? XXXXXXXXXXXXXXXXXXX

  //==== READ .ASC FILE ====// Lê .asc para uma estrutura floats
  FILE *ficheiro1;
  ficheiro1 = fopen(read_file_name,"rt"); // Inicializa ficheiro de leitura
  while (fscanf(ficheiro1, "%f", &data.number[data.num]) != EOF){ // Le ficheir linha a linha
    printf("ASC Num:%d | Name: %f \n", data.num,data.number[data.num]);
    data.num++; // Incrementa o numero de enventos
  }
  if (feof(ficheiro1)){ // End file
    data.num-=1; // Last is <null>
    fclose(ficheiro1);// Close .asc file
  }
  //=======================//
  printf("There are %d floats stored\n",data.num); //[DEBUG]

  //==== WRITE .BIN FILE ====// Converte a estrutura float para binario
  FILE *ficheiro2;
  ficheiro2 = fopen(write_file_name,"w");
  int i;
  for(i=0;i<data.num;i++){
    fp2bin(data.number[i+1], bin); // Conv float to bin
    fprintf(ficheiro2,"%s\n",bin); // Write to .bin file
  }
  fclose(ficheiro2);// Close .bin file
  //=======================//

  printf("Last nº is: %s\n",bin); //[DEBUG]




  //==== Start UNIX Connection ====//
  sock= socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock < 0) perror("Creating Socket");

	server.sun_family = AF_UNIX; // Address

  // Connect to Server
  if(connect(sock, (struct sockaddr *) &server, SUN_LEN(&server)) < 0){
      perror("Connecting to Server\n");
      return 0;
  }
  // Send data to server_ip


  close(sock);
  return 0;
}



void fp2bin_i(double fp_int, char* binString){
 int bitCount = 0;
 int i;
 char binString_temp[MANTISSA];

 do{
    binString_temp[bitCount++] = '0' + (int)fmod(fp_int,2);
    fp_int = floor(fp_int/2);
   } while (fp_int > 0);

 /* Reverse the binary string */
 for (i=0; i<bitCount; i++)
   binString[i] = binString_temp[bitCount-i-1];

 binString[bitCount] = 0; //Null terminator
}

void fp2bin_f(double fp_frac, char* binString){
 int bitCount = 0;
 double fp_int;

 while (fp_frac > 0){
    fp_frac*=2;
    fp_frac = modf(fp_frac,&fp_int);
    binString[bitCount++] = '0' + (int)fp_int;
   }
  binString[bitCount] = 0; //Null terminator
}

void fp2bin(double fp, char* binString){
 double fp_int, fp_frac;
 /* Separate integer and fractional parts */
 fp_frac = modf(fp,&fp_int);
 /* Convert integer part, if any */
 if (fp_int != 0)
   fp2bin_i(fp_int,binString);
 else
   //strcpy(binString,"0");
 //strcat(binString,"."); // Radix point
 /* Convert fractional part, if any */
 if (fp_frac != 0)
   fp2bin_f(fp_frac,binString+strlen(binString)); //Append
 else
   strcpy(binString+strlen(binString),"0");
}

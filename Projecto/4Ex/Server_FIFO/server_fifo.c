/*=========================================
Program Name :	server_fifo.c
Base Language:	Mixed
Created by   :	Esmarra
Creation Date:	19/04/2018
Rework date entries:
  *20/04/2018
Program Objectives:
Observations:
Special Thanks:
=========================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
//==== FILE I/O ====//
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h> //unit64 <-- remove?
#include <sys/types.h>
#define MANTISSA 50 // Bits Res in conv to bin
#define FLT_MAX 20 // Max lines(aka numbers) is .ASC FILE

char bin[MANTISSA];
float dflt[FLT_MAX]; // Array que da store aos floats
int max;

float bin2fp(char* digits);

int main(int argc, char **argv){
  int fifo_fd; // FIFO file descriptor
  char buffer[50]; // FIFO buffer
  float buf;
  int count;
  count=0;
  //int file_fd = open("output.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
  FILE *ficheiro1;
  if((ficheiro1 = fopen("output.asc","w")) == NULL){
    perror("\nError|Open output.asc");
    exit(EXIT_FAILURE);
  }

  if((fifo_fd=open("/tmp/myfifo",O_RDONLY)) == -1){
		perror("\nError|Open FIFO");
    exit(EXIT_FAILURE);
	}

  while(1){
    if(read(fifo_fd,buffer,sizeof(buffer)) > 0){
      if(strcmp(buffer,"flt_max")==0){
        read(fifo_fd,buffer,sizeof(buffer));// read 1 is null :/
        read(fifo_fd,buffer,sizeof(buffer));
        printf("Rcv_flt_max= %s\n",buffer );
        max=atoi(buffer);
        printf("FLT_MAX= %d\n",max);
        break;
      }
      if(strcmp(buffer,"\0")!=0){ // If its not a \0
        printf("Rcv: %s\n",buffer );
        buf=bin2fp(buffer); // Conv 2 flt
        printf("Coverted:%f\n",buf);
        dflt[count]=buf;
        count++;
      }
        //bzero(buffer,256);
        //==== PRINTF ====//
        //fprintf(ficheiro1,"%f\n",buf); //write to file
        //==== WRITE ====//
        //write(file_fd,buffer,sizeof buffer);
        //==== FWRITE ====//
        //fwrite(buffer,1,6,rtard);
        //close(file_fd);
      //break;
    }
  }
  int i;
  for(i=0;i<max;i++){
    fprintf(ficheiro1,"%f\n",dflt[i]); //write to file
  }

  //close(file_fd);
  fclose(ficheiro1);
  close(fifo_fd);
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

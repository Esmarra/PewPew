/*=========================================
Program Name :	server_fifo.c
Base Language:	Mixed
Created by   :	Esmarra
Creation Date:	19/04/2018
Rework date entries:

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

char bin[MANTISSA];
float bin2fp(char* digits);

int main(int argc, char **argv){
  int fd;
  char buffer[50];
  float buf;
  FILE *ficheiro1;
  ficheiro1 = fopen("output.asc","w");
  fd=open("/tmp/myfifo",O_RDONLY);

  while (1){
    if (read(fd,buffer,sizeof(buffer)) > 0){
      printf("%s \n", buffer);
      buf=bin2fp(buffer); // Conv 2 flt
      fprintf(ficheiro1, "%f\n",buf);
      printf("Coverted:%f\n",buf);
      exit(0);
    }
  }

  //fflush(stdin);
  //write(1,buffer,sizeof(buffer));
  //printf("\n");

  close(fd);
  return(0);
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

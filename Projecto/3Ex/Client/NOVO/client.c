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
  * Float to Bin is overkill, since all nubers will be most likely ints
Special Thanks:
=========================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Conv bin

#define MAX_CAR 30 // File Max Characters per Line
#define FLT_MAX 20 // Max lines(aka numbers) is .ASC FILE
#define MANTISSA 23 // Bits Res in conv to bin

// Conv to Binary
void fp2bin_i(double fp_int, char* binString);
void fp2bin_f(double fp_frac, char* binString);
void fp2bin(double fp, char* binString);
char bin[MANTISSA];

//Open with MMAP
#include <sys/mman.h> // MAP_PRIVATE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h> //unit64 <-- remove?

struct Data{
  float number[MAX_CAR]; // Stores in Float Array
  //float teste;
  int data_size; // Keeps Track of Array Size
};

char aux[2];

int main(){
  struct Data data[FLT_MAX]; //Start data[INDEX] struct #### DELETE ####
  //struct Data bin_data[FLT_MAX]; // #### REPLACE ####
  int line=1; //Number of Lines in file AKA FLT_MAX

  FILE *ficheiro1;
  FILE *ficheiro2;
  ficheiro1 = fopen("input.asc","rt"); // Inicializa ficheiro de leitura
  ficheiro2 = fopen("input.bin","w");
  char temp[100]; //mmap issue \!\
  char tee[MAX_CAR]={0};
  while (fgets(temp,MAX_CAR,ficheiro1) != NULL){ // Le ficheiro linha a linha
    //printf("\nRaw: %s",temp); // |DEBUG| read Raw value from file
    int j=0; //Tracks end of ASCII value per line
    int space_count=0; // Tracks Spaces per lines

    int i;
    for(i=0;i<MAX_CAR;i++){ // Sweep temp for values
      if((temp[i]==' ') || (temp[i]=='\0')){ // Spaces and Endl
        // ==== Write to Bin ==== //
        fp2bin(atof(tee), bin); // Conv float to bin
        fprintf(ficheiro2,"%s",bin); // Write to .bin file (APPEND IN SEQ = NO SPACES ??)
        //data[line].number[space_count]=atof(tee); // Store in float value #### REMOVE #####
        //printf("Tee: %s\n",tee ); // |DEBUG| shows tee string value
        tee[0]='\0'; // Wipe tee string
        j=0; //Reset ASCII value tracker
        space_count++; // Increment Space
        if(temp[i]=='\0'){  // Endl
          //printf("Last string num: %d \n",i ); // |DEBUG| shows end position in string array
          fprintf(ficheiro2,"\n"); // Write newline .bin
          break; // End sweeper
        }
      }

      if( (temp[i]!=' ') && (temp[i]!='\n')){
        tee[j]=temp[i];
        //printf("t=%c\n",tee[j] );
        j++;
      }
    }
    //data[line].data_size=space_count;
    //printf("There are %d numbers in line %d\n",data[line].data_size,line);

    /*
    for(i=0;i<data[line].data_size;i++){
      printf("Float %f\n",data[line].number[i]);
    }
    */
    line++;

  }
  line--;// Cleanup -1

  fclose(ficheiro1);// Close .asc file
  fclose(ficheiro2);// Close .bin file
  printf(">Input File has %d lines\n",line );
  temp[0]='\0'; //Reset temp

  //==== Open .bin ====// MMAP???
  const char *memblock;
  int fd;
  int flt=0;
  struct stat sb;
  fd = open("input.bin", O_RDONLY); //input.bin
  printf("Size: %lu\n",sb.st_size);
  memblock = mmap(NULL, sb.st_size,PROT_READ, MAP_PRIVATE, fd, 0);
  if (memblock == MAP_FAILED) perror("mmap");
  int i;
  for(i = 0; i < 200; i++){ // Abrir x mem blocks (MAX?)
    if(memblock[i]=='\n'){ //end strcat
      data[flt].number[0]=atof(temp);
      printf("Bin Flt: %f \n",data[flt].number[0]);
      temp[0]='\0'; //Reset temp
      //printf("endl\n" );
      flt++;
    }
    else{
      if(memblock[i]=='\0'){
        printf("dead\n" );
        break;
      }
      sprintf(aux,"%c",memblock[i]); // copy memblock to aux
      strcat(temp,aux); // clip aux to temp
    }
   }
   //==================//

  /*
  ficheiro1 = fopen("input.bin","rt");
  int flt=0;
  while (fgets(temp,MAX_CAR,ficheiro1) != NULL){
    printf("\nRaw BIN: %s",temp); // |DEBUG| read Raw value from file
    bin_data[flt].number[0]=atof(temp);
    printf("Bin Flt: %f\n",bin_data[flt].number[0]);
    flt++;
  }
  */

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

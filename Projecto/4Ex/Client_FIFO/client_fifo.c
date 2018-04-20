/*=========================================
Program Name :	client_fifo.c
Base Language:	Mixed
Created by   :	Esmarra
Creation Date:	20/04/2018
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
#include <math.h> // Conv bin
//====  MMAP ====//
#include <sys/mman.h> // MAP_PRIVATE
#include <sys/types.h>
#include <stdint.h> //unit64 <-- remove?
//==== FILE I/O ====//
#include <sys/stat.h>
#include <fcntl.h>

#define FLT_MAX 20 // Max lines(aka numbers) is .ASC FILE
#define MANTISSA 50 // Bits Res in conv to bin

// Conv to Binary
void fp2bin_i(double fp_int, char* binString);
void fp2bin_f(double fp_frac, char* binString);
void fp2bin(double fp, char* binString);
char bin[MANTISSA];

float dbin[FLT_MAX]; // Array que da store aos floats

int main(int argc, char **argv){
  int line=1; //Number of Lines in file AKA FLT_MAX
  int fifo_fd;
  FILE *ficheiro1;
  FILE *ficheiro2;
  ficheiro1 = fopen("input.asc","rt"); // Inicializa ficheiro de leitura
  ficheiro2 = fopen("input.bin","w");
  float temp;
  while (fscanf(ficheiro1,"%f",&temp) != EOF){ // Le ficheiro linha a linha
    printf("Raw: %f\n",temp); // |DEBUG| read Raw value from file
    fp2bin(temp,bin); // Conv float to bin
    printf("Bin: %s\n",bin);
    //fwrite(&bin, sizeof(bin), 1, ficheiro2); // ENCODING ERROR
    fprintf(ficheiro2,"%s\n",bin);
    //fprintf(ficheiro2,"\n");
    line++;
  }
  line--;// Cleanup -1
  fclose(ficheiro1);// Close .asc file
  fclose(ficheiro2);// Close .bin file
  printf(">Input File has %d lines\n",line );

  //==== Open .bin ====//   ########## BUGGADO ##########
  char buffer[100];
  int flt=0; // Number of Floats/lines in .bin
  /*
  FILE *fp;
  fp = fopen("input.bin", "rb");
  fread(buffer, sizeof(double), 1, fp);
  printf("wtf:%s endl\n", buffer);
  fclose(fp);
  */
  const char *memblock;
  int fd;
  size_t size;
  fd = open("input.bin", O_RDONLY); //input.bin
  //printf("Size: %zd\n",sb.st_size);
  size = lseek(fd, 0, SEEK_END); // Find Max Size
  lseek(fd, 0, SEEK_SET); // Reset cursor 0
  printf("File Size: %zd bytes\n",size);
  memblock = mmap(NULL, size,PROT_READ, MAP_PRIVATE, fd, 0);
  if (memblock == MAP_FAILED){
    perror("\nError|mmap");
    exit(EXIT_FAILURE);
  }
  char aux[2];
  int line_size[5]; // Cheat (we know how many lines file has)
  int i;
  for(i = 0; i < size+1; i++){ // Abrir <FILE SIZE> mem blocks
    if(memblock[i]=='\n'){ //end strcat
      dbin[flt]=atof(buffer); // Conv float e store num flt array
      printf("\nBin %d: %.10f \n",flt,dbin[flt]);
      printf("Bin Size: %d\n",line_size[flt]);
      buffer[0]='\0'; //Reset temp
      flt++; // Increment flt (=FLT_MAX)
    }
    else{
      if(memblock[i]=='\0'){
        printf("End file\n" ); // <-- We never get here (use size+1) to get here
        break;
      }
      sprintf(aux,"%c",memblock[i]); // copy data in memblock to aux
      strcat(buffer,aux); // clip aux to temp
      //printf("aux:%s\n",buffer);
      line_size[flt]++; // Incrment size of each line
    }
   }
   close(fd);
  //==== XXXXXXXX ====// ########## BUGGADO ##########

  //==== START FIFO ====//
  mkfifo("/tmp/myfifo", 0666); // CRIA FIFO
  fifo_fd=open("/tmp/myfifo",O_WRONLY);
  for(i=0;i<flt;i++){ // Envia data
    snprintf(buffer, sizeof buffer, "%.10f", dbin[i]); // Copy float(dbin) to char(buffer)
    write(fifo_fd,buffer,sizeof(buffer));
    //buffer[0]='\0'; //Reset temp
  }
  //== Send flt_max flag ==//
  strcpy(buffer,"flt_max");
  write(fifo_fd,buffer,sizeof(buffer));

  //== Send flt_max value ==//
  snprintf(buffer, sizeof buffer, "%d", flt); // Copy float(dbin) to char(buffer)
  write(fifo_fd,buffer,sizeof(buffer));

  exit(0);
}

void fp2bin_i(double fp_int, char* binString){
 int bitCount = 0;
 int i;
 char binString_temp[MANTISSA];
 do{
    binString_temp[bitCount++] = '0' + (int)fmod(fp_int,2);
    fp_int = floor(fp_int/2);
   } while (fp_int > 0);
 for (i=0; i<bitCount; i++) // Inverter bin
   binString[i] = binString_temp[bitCount-i-1];

 binString[bitCount] = 0; // null
}

void fp2bin_f(double fp_frac, char* binString){
 int bitCount = 0;
 double fp_int;
 while (fp_frac > 0){
    fp_frac*=2;
    fp_frac = modf(fp_frac,&fp_int);
    binString[bitCount++] = '0' + (int)fp_int;
   }
  binString[bitCount] = 0; // null
}

void fp2bin(double fp, char* binString){
 double fp_int, fp_frac;
 fp_frac = modf(fp,&fp_int); // Separar int das frac
 if (fp_int != 0) // Convert int
   fp2bin_i(fp_int,binString);
 else
   strcpy(binString,"0");
 strcat(binString,"."); // Ponto
 if (fp_frac != 0)  // Convert fracção
   fp2bin_f(fp_frac,binString+strlen(binString)); // Append
 else
   strcpy(binString+strlen(binString),"0");
}

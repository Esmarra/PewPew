#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Conv bin
//==== MSG QUEQUE ====//
#include <sys/ipc.h>
#include <sys/msg.h>
//====  MMAP ====//
#include <sys/mman.h> // MAP_PRIVATE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h> //unit64 <-- remove?
#include <errno.h>
#include <unistd.h>

#define FLT_MAX 20 // Max lines(aka numbers) is .ASC FILE
#define MANTISSA 50 // Bits Res in conv to bin

#define MAX_BYTES	  20
// Conv to Binary
void fp2bin_i(double fp_int, char* binString);
void fp2bin_f(double fp_frac, char* binString);
void fp2bin(double fp, char* binString);
char bin[MANTISSA];

float dbin[FLT_MAX]; // Array que da store aos floats

typedef struct{ // MQ
  long mtype;
  char mtext[MAX_BYTES+1]; // 1 for the '\0' string terminator
}message_buf;


char aux[2];

int main(){
  int line=1; //Number of Lines in file AKA FLT_MAX !REMOVE????!

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

  //==== Open .bin ====//
  char buffer[100];
  int flt=0; // Number of Floats/lines in .bin
  const char *memblock;
  int fd;
  size_t size;
  fd = open("input.bin", O_RDONLY); //input.bin
  size = lseek(fd, 0, SEEK_END); // Find Max Size
  lseek(fd, 0, SEEK_SET); // Reset cursor 0
  printf("File Size: %zd bytes\n",size);
  memblock = mmap(NULL, size,PROT_READ, MAP_PRIVATE, fd, 0);
  if (memblock == MAP_FAILED){
    perror("\nError|mmap");
    exit(EXIT_FAILURE);
  }
  char aux[2];
  int i;
  for(i = 0; i < size+1; i++){ // Abrir <FILE SIZE> mem blocks
    if(memblock[i]=='\n'){ //end strcat
      dbin[flt]=atof(buffer); // Conv float e store num flt array
      printf("Bin %d: %.10f \n",flt,dbin[flt]);
      //printf("Bin Size: %d\n",line_size[flt]);
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
    }
   }
   close(fd);
  //==== XXXXXXXX ====//

   //==== Message Queues ====//
   message_buf buf;
   key_t key;
   int msq_id; // message queue ID

   if((key = ftok("/bin",1234)) == -1){
     perror("Error|Key ftok");
     exit(1);
   }
   if((msq_id = msgget(key, 0)) == -1){
     if(errno != ENOENT){
       perror("Error|Queue 1st message");
       exit(EXIT_FAILURE);
     }
   }

   /*================ BUF FLAGS =================*/
   /* 1-Float Value  |  2-FLT_MAX  &End of comms */
   /*============================================*/
   for(i=0;i<=flt;i++){ // ver melhor bytes approach
     if(i==flt){ // Send max car? DEADBUG
       buf.mtype = 2; // FLT_MAX flag
       sprintf(buf.mtext,"%d",flt);
       //printf("FLT=%d\n",flt); // DEBUG
       printf(">Sending=%s\n",buf.mtext );
     }
     else{
       buf.mtype = 1;
 		   buf.mtext[MAX_BYTES] = '\0'; //clear string
       printf(">Sending: %.10f\n",dbin[i]);
       sprintf(buf.mtext,"%.10f",dbin[i]);
     }


     if (msgsnd(msq_id,(message_buf *)&buf,sizeof(message_buf) - sizeof(long),0) == -1){
       perror("Error|Send Msg [SERVER DOWN]");
       exit(EXIT_FAILURE);
 	   }
     printf("<Sent: %s\n",buf.mtext);
   }

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
   strcpy(binString,"0");
 strcat(binString,"."); // Radix point
 /* Convert fractional part, if any */
 if (fp_frac != 0)
   fp2bin_f(fp_frac,binString+strlen(binString)); //Append
 else
   strcpy(binString+strlen(binString),"0");
}

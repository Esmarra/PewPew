#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
//==== MSG QUEQUE ====//
#include <sys/ipc.h>
#include <sys/msg.h>
//==== FILE I/O ====//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h> //unit64 <-- remove?
#include <errno.h>
#define MANTISSA 50 // Bits Res in conv to bin
#define MAX_BYTES 20

char bin[MANTISSA];
float bin2fp(char* digits);

typedef struct{ // MQ
  long mtype;
  char mtext[MAX_BYTES+1]; // 1 for the '\0' string terminator
}message_buf;

int main(int argc, char **argv){
  //==== Message Queues ====//
  message_buf buf;
  key_t key;
  int msq_id; // message queue ID
  float buf_flt; // Stores buff in float (Corresponds to buf in Server.c from Assignment 1)

  if((key = ftok("/bin",1234)) == -1){
    perror("Error|Key ftok");
    exit(EXIT_FAILURE);
  }
  if ((msq_id = msgget(key,IPC_CREAT|0666)) == -1){ // Create Queue Appendable by all
   perror("Error|Create Q msgget");
   exit(EXIT_FAILURE);
  }

  FILE *ficheiro1;
  ficheiro1 = fopen("output.asc","w");

  if((msq_id = msgget(key, 0)) == -1){ // Get Msg Q
    if(errno != ENOENT){
      perror("Error|Queue 1st message");
      exit(EXIT_FAILURE);
    }
  }
  printf("[Server Running]:\n");
  /*================ BUF FLAGS =================*/
  /* 1-Float Value  |  2-FLT_MAX  &End of comms */
  /*============================================*/
  do{
    if ((msgrcv(msq_id,(message_buf *)&buf,sizeof(message_buf) - sizeof(long),0,0)) == -1){ // Get Msg
      perror("Error|Get Msg");
      exit(EXIT_FAILURE);
	  }
    //==== Msg Processing ====// // RECHECK! Should work now
    if(buf.mtype==1){ //Float Values -> Write to File
      buf_flt=bin2fp(buf.mtext); // Conv 2 flt
      fprintf(ficheiro1,"%f\n",buf_flt); //write to file
      printf("I'v got:%s\n",buf.mtext);
    }
    if(buf.mtype==2){ // FLT_MAX value
      printf("FLT_MAX=%s\n",buf.mtext);
      //podiamos fazer break e mandamos escrevemos menos 1 valor do lado do client
    }

  }while(buf.mtype != 2);

  if (msgctl(msq_id,IPC_RMID,NULL) == -1){
    perror("Error|Clear Q");
    exit(EXIT_FAILURE);
  }
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

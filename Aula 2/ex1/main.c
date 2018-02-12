/*=========================================
Program Name :	 Prof Code Criar Processo
Base Language:	 English
Created by   :	 Esmarra
Creation Date:	 12/02/2018
Rework date entries:
Program Objectives:
Observations:
* Based on: P.335 by Araujo
* Git Repo: https://github.com/Esmarra/Electricjam
Special Thanks:
=========================================*/
#include <stdio.h>   /* printf, stderr, fprintf */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <sys/wait.h> // Wait
#include <sys/types.h> /* pid_t */

int main (int argc, char *argv[]){
  printf("Hello World\n");
  pid_t childpid;
  int status;
  int i;
  for(i=0;i<10;i++){
    printf("I am father %d\n",i);
  }
  childpid = fork();
  //if (childpid == 0)printf("Hello World\n");
  if(childpid == -1){ // Erro
      perror("Failed to fork");
      return 1;
    }
  if (childpid == 0){ // Estamos no Child
    int i;
    for(i=0;i<10;i++){
      printf("I am child %d\n",i);
    }
    //printf("My PID: %ld\n",(long)getpid());
  }
  else{ // Parent
    wait(&status);
    printf("Parent PID: %ld Child PID: %ld\n",(long)getpid(),(long)childpid);
  }
  //return 0;
}


/*=========================================
Program Name :	main.c
Base Language:	Mixed
Created by   :	Esmarra
Creation Date:	11/04/2018
Rework date entries:
  * 18/04/2018
Program Objectives:
  * do ls -1 and send result to output.txt
  * do cat - /etc/passwd with output.txt as input
  * do sort -r with input from cat
Observations:
Special Thanks:
=========================================*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h> // pid_t
#include <sys/socket.h>
#include <sys/un.h>

void child1(); // Execute ls
void child2(int *); // Execute cat
void parent(pid_t *,int *); // Execute sort

int main(int argc, char **argv){
  pid_t pid[2]; // Get current process pid
  int pipe_fd[2]; // Pipe File Descriptor 0-Opened for Reading | 1-Opened for Writing
  int status[2];

  if(pipe(pipe_fd) == -1){ // Start Pipe cat-sort
    perror("\nError|Pipe Down");
    exit(EXIT_FAILURE);
  }

  if((pid[0]=fork()) == 0){
    //printf("\n Child 1");
    child1();
  }
  else if(pid[0] == -1){
    perror("\nError|Fork[0]");
    exit(EXIT_FAILURE);
  }
  else{
    if(((pid[1]=fork()) == 0)&&(waitpid(pid[1],&status[0],0))){ // forks and wait for child 1 to end
      //printf("\n Child 2");
      child2(pipe_fd);
    }
    else if(pid[1] == -1){
      perror("\nError|Fork[1]");
      exit(EXIT_FAILURE);
    }
    else{
      //printf("\n Parent");
      parent(pid,pipe_fd);
      exit(EXIT_SUCCESS);
    }
  }
  exit(EXIT_SUCCESS);
}

void child1(){ // ls
  int file_fd;
  //==== Changing ls output ====//
  if( (file_fd = open("output.txt",O_WRONLY|O_CREAT|O_TRUNC,0666)) == -1){
    perror("\nError|Child1 Open File");
    exit(EXIT_FAILURE);
  }
  if(dup2(file_fd,1) == -1){
    perror("\nError|Child1 dup2");
    exit(EXIT_FAILURE);
  }
  if(close(file_fd) == -1){// Close File descriptor do ficheiro
    perror("\nError|Child1 Close File");
    exit(EXIT_FAILURE);
  }
  //==== Executar Comando ====//
  if(execlp("ls","ls","-1",NULL) == -1){ // Fazer ls
    perror("\nError|Child1 execlp ls");
    exit(EXIT_FAILURE);
  }
}

void child2(int *pipe_fd){ // cat
  int file_fd;
  //==== Changing cat input ====//
  if((file_fd = open("output.txt", O_RDONLY)) == -1){
    perror("\nError|Child2 Open File");
    exit(EXIT_FAILURE);
  }
  if(dup2(file_fd,0) == -1){
    perror("\nError|Child2 dup2");
    exit(EXIT_FAILURE);
  }
  if(close(file_fd) == -1){// Close File descriptor do ficheiro
    perror("\nError|Child2 Close File");
    exit(EXIT_FAILURE);
  }

  //==== Changing cat output ====//
  if(close(pipe_fd[0]) == -1){ // Close Pipe Read
    perror("\nError|Child2 Close Pipe Read");
    exit(EXIT_FAILURE);
  }
  if(dup2(pipe_fd[1],1) == -1){
    perror("\nError|Child2 dup2");
    exit(EXIT_FAILURE);
  }
  if(close(pipe_fd[1]) == -1){
    perror("\nError|Child2 Close Pipe Write");
    exit(EXIT_FAILURE);
  }

  //==== Executar Comando ====//
  if(execlp("cat","cat","-","/etc/passwd",NULL) == -1){ // Faz cat
    perror("\nError|Child2 execlp cat");
    exit(EXIT_FAILURE);
  }
}

void parent(pid_t *pid,int *pipe_fd){ // sort
  int endp; // Process end
  int status[2]; // Childs state

  int i;
  for(i=0;i<sizeof(pid)/sizeof(pid_t);i++){
    while( (endp=waitpid(pid[i],&status[i],0)) != pid[i]){ // Loops until childs complete
      if(endp == -1){
        perror("\nError|Parent Wait");
        exit(EXIT_FAILURE);
      }
    }
  }

  //==== Change sort input ====//
  if (close(pipe_fd[1]) == -1){ // Close Pipe Write
		perror("\nError|Parent Close Pipe Write");
    exit(EXIT_FAILURE);
	}
  if (dup2(pipe_fd[0], 0) == -1){
		perror("\nError|Parent dup2");
    exit(EXIT_FAILURE);
	}
  if (close(pipe_fd[0]) == -1){ // Close Pipe Read
    perror("\nError|Parent Close Pipe Read");
    exit(EXIT_FAILURE);
  }

  //==== Executar Comando ====//
  if (execlp("sort","sort","-r",NULL) == -1){
    perror("\nError|Parent execlp sort");
    exit(EXIT_FAILURE);
  }
}

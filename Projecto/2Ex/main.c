
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

#define RED "\x1B[31m"
#define GREEN	"\x1B[32m"
#define WHITE "\x1B[00m"

void child1(int *);

int main(int argc, char **argv){

  pid_t pid[2]; //get current process pid
  // Pipe [1] send from Parent to Child
  // Pipe [2] send from Child to Parent
  int pipe_fd1[2]; // Pipe[1] File Descriptor 0-Opened for Reading | 1-Opened for Writing (Stores 2 Values)
  int pipe_fd2[2]; // Pipe[2] File Descriptor

  pid[0] = fork();
  pid[1] = fork();
  //printf("current pid=%jd\n",(intmax_t) pid[0]);
  // ==== Error Check ==== //
  if (pipe(pipe_fd1) == -1){
    perror("\n Pipe[1] Down");
  }
  if (pipe(pipe_fd2) == -1){
    perror("\n Pipe[2] Down");
  }

  if (pid[0] == -1){
    perror("\n Fork[0]");
  }
  if (pid[1] == -1){
    perror("\n Fork[1]");
  }
  // ====             ==== //

  if (pid[0] == 0){
    printf("This is Child[1]\n");
    child1(pipe_fd1); // cat
    /*
    //==== TESTING ====//
    int varc=0;
    while (1) {
      printf("%sChild_count=%d\n",RED,varc);
      varc++;
      sleep(1);
    }
    //==== XXXXXX ====//
    */
  }
  else{
    if(pid[1]==0){
      child2(pipe_fd); // sort
    }
    //fechar pipe?????
  }

  else{
    printf("This is Parent\n");
    parent(pipe_fd1);
    /*
    //==== TESTING ====//
    int varp=0;
    while (1) {
      printf("%sParent_count=%d\n",GREEN,varp);
      //system("ls"); // Uses Command but Process Continues \CANT USE|
      //execlp("ls", "ls", NULL); // Uses Command KILL's Process
      varp++;
      sleep(5);
    }
    //==== XXXXXX ====//
    */
    //close(pipe_fd1[0]);// Close reading end of first pipe
    //WRITE????
  }
}

void parent(int *pipe_fd){ // ls pipe
  if (close(pipe_fd[0]) == -1){ // Close Pipe Read
		perror("parent-close Leitura");
	}
  if (dup2(pipe_fd[1], STDOUT_FILENO) == -1){ // Copy of the file descriptor
		perror("parent-dup2");
	}
  if (close(pipe_fd[1]) == -1){ // Close Pipe Write
    perror("parent-close Escrita");
  }
  // Fazer ls
  if (execlp("ls","ls","-1",NULL) == -1){
    perror("parent-execlp ls");
  }
}

void child1(int *pipe_fd){ // cat pipe
  if (close(pipe_fd[PIPE_INPUT]) == -1){// Close Pipe Read
    perror("child1-close Leitura");
  }
  if (dup2(pipe_fd[PIPE_OUTPUT], STDOUT_FILENO) == -1){ // Copy of the file descriptor
    perror("child1->dup2");
  }
  if (close(pipe_fd[PIPE_OUTPUT]) == -1){// Close Pipe Write
    perror("child1-close Escrita");
  }
  // Fazer cat
  if (execlp("cat","cat","/etc/passwd",NULL) == -1){ // <-- INCOMPLETO
    perror("child1-execlp-cat");
  }
}

void child2(int *pipe_fd){ // sort pipe
  if (close(pipe_fd[PIPE_INPUT]) == -1){// Close Pipe Read
    perror("child2-close Leitura");
  }
  if (dup2(pipe_fd[PIPE_OUTPUT], STDOUT_FILENO) == -1){ // Copy of the file descriptor
    perror("child2->dup2");
  }
  if (close(pipe_fd[PIPE_OUTPUT]) == -1){// Close Pipe Write
    perror("child2-close Escrita");
  }
// Fazer Sort
  if (execlp("sort","sort","-r",NULL) == -1){
    perror("child2-execlp-sort");
  }
}

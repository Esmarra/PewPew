
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

void child(int *);

int main(int argc, char **argv){

  pid_t pid[1]; //get current process pid
  int pipe_fd[2]; // Pipe File Descriptor 0-Opened for Reading | 1-Opened for Writing

  pid[0] = fork();
  printf("current pid=%jd\n",(intmax_t) pid[0]);
  // ==== Error Check ==== //
  if (pipe(pipe_fd) == -1){
    perror("\n Pipe[0] Down");
  }
  if (pid[0] == -1){
    perror("\n Fork[0]");
  }
  // ====             ==== //

  else if (pid[0] == 0){
    printf("This is Child\n");
    child(pipe_fd);

    //==== TESTING ====//
    int varc=0;
    while (1) {
      printf("%sChild_count=%d\n",RED,varc);
      varc++;
      sleep(1);
    }
    //==== XXXXXX ====//
  }
  else{
    printf("This is Parent\n");

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
  }
}

void child(int *pipe_fd){ // ls pipe
  if (close(pipe_fd[0]) == -1){ // Close Pipe Read
		perror("Child Pipe close Leitura");
	}
  if (dup2(pipe_fd[1], STDOUT_FILENO) == -1){ // Copy of the file descriptor
		perror("Child dup2");
	}
  if (close(pipe_fd[1]) == -1){ // Close Pipe Write
    perror("Child Pipe close Escrita");
  }
  // Fazer ls & kill process?
  if (execlp("ls", "ls","-1", NULL) == -1){
    perror("child @ execlp @ ls");
  }
}

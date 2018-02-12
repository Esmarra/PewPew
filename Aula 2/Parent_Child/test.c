#include <stdio.h>   /* printf, stderr, fprintf */
#include <sys/types.h> /* pid_t */
#include <unistd.h>  /* _exit, fork */
#include <stdlib.h>  /* exit */
#include <errno.h>   /* errno */

int main(void){
  printf("Hello World\n" );
   pid_t  pid;
   /* Output from both the child and the parent process
    * will be written to the standard output,
    * as they both run at the same time.
    */
   pid = fork();
   printf("current pid=%d\n",pid );
   if (pid == -1){ // Detects error in fork()
      /* Error:
       * When fork() returns -1, an error happened
       * (for example, number of processes reached the limit).
       */
      fprintf(stderr, "can't fork, error %d\n", errno);
      exit(EXIT_FAILURE);
   }
   else if (pid == 0){ // Estamos no Child
      /* Child process:
       * When fork() returns 0, we are in
       * the child process.
       */
      int  j;
      for (j = 0; j < 10; j++){
         printf("Noooo: %d\n", j);
         sleep(.1);
      }
      _exit(0);  /* Note that we do not use exit() */
   }
   else{
      /* When fork() returns a positive number, we are in the parent process
       * (the fork return value is the PID of the newly created child process)
       * Again we count up to ten.
       */
      int  i;
      for (i = 0; i < 10; i++){
         printf("Luke i'm your father: %d\n", i);
         sleep(.1);
      }
      exit(0);
   }
   return 0;
}

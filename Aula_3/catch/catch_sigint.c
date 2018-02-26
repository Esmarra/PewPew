# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
#include <unistd.h> //Sleep
void sigproc (int var );
int main (){
  signal (SIGINT , sigproc );
  while (1){
    sleep (1);
    printf ("I'm running \n");
  }; /* infinite loop */
  return 0;
}
void sigproc (int var ){
  /*--------------------------------------
  NOTe : some versions of UNIX will reset
  signal to default after each call . So for
  portability reset signal each time */
  signal (SIGINT , sigproc );
  /*------------------------------------*/
  printf ("You have pressed ctrl -c\n");
  exit (0); /* normal exit status */
}

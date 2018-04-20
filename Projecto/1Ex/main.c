# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
#include <unistd.h> //Sleep
#define RED "\x1B[31m"
#define GREEN	"\x1B[32m"
#define WHITE "\x1B[00m"

void sigproc (int var );
void sigqproc (int var );

int main (int argc, char **argv){
  signal (SIGINT , sigproc );
  signal (SIGQUIT , sigqproc );
  while (1){ // Loop
    printf ("%s>I'm running\n",WHITE);
    sleep (1);
  };

  return 0;
}

void sigproc (int var ){
  signal (SIGINT , sigproc );
  printf ("\n>You have pressed Ctrl-C\n");
  printf ("%s>==== EXIT BLOCKED ====\n",RED);
  //exit (0);
}

void sigqproc (int var ){
  signal (SIGQUIT , sigqproc );
  printf ("%s\n>You have pressed Ctrl-\\ \n",WHITE);
  printf ("%s>==== EXIT COMPLETE ====\n",GREEN);
  exit (0);
}

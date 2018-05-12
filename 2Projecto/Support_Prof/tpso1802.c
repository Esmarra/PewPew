/*-----------------------------------------------------------------------*/
/*                  tpso1802.c                                           */
/*-----------------------------------------------------------------------*/
/*  Compilation: make                                                    */
/*-----------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "semlib.h"

/*-----------------------------------------------------------------------*/
/*                  macro definitions                                    */
/*-----------------------------------------------------------------------*/
#define NUM_ESCR 22
#define NUM_LEIT 20
#define LIST_SIZE 500
#define MUTEX 0
#define STOP_WRITERS 1

/*-----------------------------------------------------------------------*/
/*                  type definitions                                     */
/*-----------------------------------------------------------------------*/
typedef struct{
  int tail;
  int head;
  int readers;
  int slots[LIST_SIZE];
} mem_structure;

/*-----------------------------------------------------------------------*/
/*                  global variables definitions                         */
/*-----------------------------------------------------------------------*/
int semid, shmid;
pid_t childs[NUM_ESCR + NUM_LEIT];

/*-----------------------------------------------------------------------*/
/*                  cleanup()                                            */
/*-----------------------------------------------------------------------*/
void cleanup(int signo){

  int i = 0;
  while (i < (NUM_ESCR + NUM_LEIT))
    kill(childs[i++], SIGKILL);
  while (wait(NULL) != -1) ;

  if (semid >= 0)
    semctl(semid, 0, IPC_RMID);
  if (shmid >= 0)
    shmctl(shmid, IPC_RMID, NULL);
  exit(0);
}


/*-----------------------------------------------------------------------*/
/*                  next()                                               */
/*-----------------------------------------------------------------------*/
int next(int pos){

  return (pos + 1) % LIST_SIZE;  
}

/*-----------------------------------------------------------------------*/
/*                  get_code()                                           */
/*-----------------------------------------------------------------------*/
int get_code(){

  return 1 + (int) (1000.0 * rand() / (RAND_MAX + 1.0));
}

/*-----------------------------------------------------------------------*/
/*                  do_write()                                           */
/*-----------------------------------------------------------------------*/
void do_write(int n_writer, mem_structure *queue){

  queue->slots[queue->head] = get_code();
  fprintf(stderr, "CENTRAL: %d received call %d at position %d.\n", 
          n_writer, queue->slots[queue->head], queue->head);
  queue->head = next(queue->head);
}

/*-----------------------------------------------------------------------*/
/*                  writer()                                             */
/*-----------------------------------------------------------------------*/
int writer(int n_writer){

  mem_structure *queue;

  if ((queue = shmat(shmid, NULL, 0)) == (mem_structure *) -1) {
    perror("error in the writer shmat");
    exit(1);
  }

  srand(getpid());

  while (1) {

    /*insert code here*/

    sleep(1 + (int) (10.0 * rand() / (RAND_MAX + 1.0)));
  }

  exit(0);
}

/*-----------------------------------------------------------------------*/
/*                  do_read()                                            */
/*-----------------------------------------------------------------------*/
void do_read(int pos, int n_reader, mem_structure *queue){

  if (pos != queue->head)
    fprintf(stderr, "OPERATOR %d answered call %d from position %d.\n", 
            n_reader, queue->slots[pos], pos);
  else
    fprintf(stderr, "OPERADOR %d - There are no calls.\n", n_reader);
}

/*-----------------------------------------------------------------------*/
/*                  reader()                                             */
/*-----------------------------------------------------------------------*/
int reader(int n_reader){

  mem_structure *queue;
  int local_tail;

  if ((queue = shmat(shmid, NULL, 0)) == (mem_structure *) -1) {
    perror("error in the reader shmat");
    exit(1);
  }

  srand(getpid());

  while (1) {

    /*insert code here*/
  
    sleep(1 + (int) (10.0 * rand() / (RAND_MAX + 1.0)));
  }
  exit(0);
}


/*-----------------------------------------------------------------------*/
/*                  monitor()                                            */
/*-----------------------------------------------------------------------*/
int monitor(mem_structure *queue){

  int aux;
  struct sigaction act;

  act.sa_handler = cleanup;
  act.sa_flags = 0;
  if ((sigemptyset(&act.sa_mask) == -1) || 
    (sigaction(SIGINT, &act, NULL) == -1)) 
    perror("Failed to set SIGINT to handle Ctrl-C.");

  while (1) {

    sem_wait(semid, STOP_WRITERS);
    
    aux = queue->tail == queue->head ? 0 : 
          queue->tail  < queue->head ? queue->head - queue->tail : 
                                   LIST_SIZE - (queue->tail - queue->head);
    fprintf(stderr, "\nThere are %d calls waiting.\n\n", aux);

    sem_signal(semid, STOP_WRITERS);
    sleep(5);
  }
  return(0);
}

/*-----------------------------------------------------------------------*/
/*                  main()                                               */
/*-----------------------------------------------------------------------*/
int main(){

  int i = 0;
  int j = 0;
  mem_structure *queue;

  /*insert code here*/

  queue->tail = 0;
  queue->head = 0;
  queue->readers = 0;

  while (i < NUM_ESCR) {
    if ((childs[j] = fork()) == 0) {
      writer(i);
      exit(0);
    } else if (childs[j] == -1) {
      perror("error creating a writer process");
      while (wait(NULL) != -1) sleep(1);
      exit(1);
    }
    i++;
    j++;
  }
  i = 0;
  while (i < NUM_LEIT) {
    if ((childs[j] = fork()) == 0) {
      reader(i);
      exit(0);
    } else if (childs[j] == -1) {
      perror("error creating a reader process");
      while (wait(NULL) != -1) sleep(1);
      exit(1);
    }
    i++;
    j++;
  }
  
  monitor(queue);
  exit(0);
}

/*-----------------------------------------------------------------------*/


# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <unistd.h> //Sleep
int main(void){ //here
  long i,j;
  double soma;
  double arraysoma[5000];
  double* ponteiro;
  ponteiro=arraysoma;
  for ( j=0; j<10000; j+=2){
    soma=0;
    for (i=0;i<20000;i++){ //Here
        soma+=1.0/(i+j);
    }
    *ponteiro=soma;
    ponteiro++;
  }
  for (j=0; j<=5000;j++){
    printf("%f \n",arraysoma[j]);
  }
  sleep(60);
  printf("Estou pronto para terminar, prima [ENTER] para sair.\n");
  getchar();
}

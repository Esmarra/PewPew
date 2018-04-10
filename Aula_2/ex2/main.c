#include <stdio.h>   /* printf, stderr, fprintf */
#include <stdlib.h>  /* exit */
int iexit; //Keep user in menu (var)
char c_exit; //Exit Menu aka Program
int menu_choice; //Menu choice (var)

int main(int argc, char *argv[]){
  iexit=0; //Forces Enter Menu
  menu_choice=6;
  char yes;

  system("clear");
  while(iexit!=1){
    system("clear"); //Clear Window
    //===== Menu ====//
    printf(" Menu:\n");
    printf("  0: Exit\n");
    printf("  1: Edit a file\n");
    printf("  2: View file\n");
    printf("  3: List file\n");
    printf("  4: Remove file\n");
    //===============//
    printf(" Choice: ");
  	scanf("%d",&menu_choice); //Read Input

    switch(menu_choice){
      case 0:
      system("clear");
      printf(" Are you sure you want to exit?(y/n): ");
      scanf("%s",&c_exit);
      if(c_exit=='y'){ //Confirm_exit
          printf(" Terminating Program...\n");
          iexit=1; //Exits Menu, and ends Program
      }
      break;

      case 1:
      system("clear");
      printf("==== Edit_File ====\n");
      printf("\n Return to Menu(y/n):");
  		scanf("%s",&yes); //Exit Case
      break;

      case 2:
      system("clear");
      printf("==== View_File ====\n");
      printf("\n Return to Menu(y/n):");
  		scanf("%s",&yes); //Exit Case
      break;

      case 3:
      system("clear");
      printf("==== List_File ====\n");
      printf("\n Return to Menu(y/n):");
  		scanf("%s",&yes); //Exit Case
      break;

      case 4:
      system("clear");
      printf("==== Remove_File ====\n");
      printf("\n Return to Menu(y/n):");
  		scanf("%s",&yes); //Exit Case
      break;
      menu_choice=6; // Reset Menu Coice
    }
  }
  //=============//
  exit(0);
  return 0;
}

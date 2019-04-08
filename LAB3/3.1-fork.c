#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include<sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include<pthread.h>

sem_t *mySem1 = NULL;
sem_t *mySem2 = NULL;
sem_t *mySem3 = NULL;

int main(int argc, char *argv[])
{
  pid_t P2,P3,P4;
  mySem1 = sem_open("mySem1", O_CREAT, 0666, 0);
  mySem2 = sem_open("mySem2", O_CREAT, 0666, 0);
  mySem3 = sem_open("mySem3", O_CREAT, 0666, 0);
   //创建P1子进程
  P2 = fork();
  P3 = fork();
  P4= fork();


  printf("I am the process P1\n");
  sem_post(mySem1);

  if(P2 == 0){


  sem_wait(mySem1);

   printf("I am the process P2\n");
   sem_post(mySem2);
   sem_post(mySem1);}

  else if(P3 == 0){
  sem_wait(mySem1);

   printf("I am the process P3\n");
   sem_post(mySem3);
   sem_post(mySem1);}

   else if(P4 == 0){
  sem_wait(mySem2);
  sem_wait(mySem3);
   printf("I am the process P4\n");}

sem_close(mySem1);
sem_close(mySem2);
sem_close(mySem3);
sem_unlink("mySem1");
sem_unlink("mySem2");
sem_unlink("mySem3");
return 0;
}







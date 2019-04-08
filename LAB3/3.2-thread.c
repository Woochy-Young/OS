#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
 #include<unistd.h>
 #include<sys/types.h>
 #include <sched.h>


volatile int ticketCount = 1000;

sem_t *mySem = NULL;

 void *sold()
  {
       int temp;

     sem_wait(mySem);
      temp=ticketCount;
     pthread_yield();
    temp=temp-1;
   pthread_yield();
   ticketCount=temp;
printf("卖出1，剩余%d\n",ticketCount);
      sem_post(mySem);

 }

  void *cancel()
  {
int temp;


       sem_wait(mySem);
      temp=ticketCount;
     pthread_yield();
     temp=temp+1;
     pthread_yield();
     ticketCount=temp;
printf("退还1，剩余%d\n",ticketCount);
     sem_post(mySem);

 }

 int main(int argc, char *argv[])
 {
 pthread_t p1, p2;
int i=100;
 mySem = sem_open("mySem", O_CREAT, 0666, 1);
 printf("Initial value : %d\n", ticketCount);
while(i--){
 pthread_create(&p1, NULL, sold, NULL);
 pthread_create(&p2, NULL, cancel, NULL);

 pthread_join(p1, NULL);

 pthread_join(p2, NULL);}

 printf("Final value : %d\n", ticketCount);

   sem_close(mySem);
 sem_unlink("mySem");
return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include<unistd.h>
#include<sys/types.h>

char buf[10];
int i=0,j=0;
sem_t *empty = NULL;
sem_t *full = NULL;

 void *out()
  {
    for(;;i++){
    sleep(1);
    sem_wait(full);
printf("%c\n",buf[i%10]);
      sem_post(empty);
    }
 }

  void *in()
  {
  for(;;j++){
       sem_wait(empty);
scanf("%c",&buf[j%10]);
     sem_post(full);
      }
 }

 int main(int argc, char *argv[])
 {
  pthread_t p1, p2;
  empty = sem_open("empty", O_CREAT, 0666, 10);
  full = sem_open("full", O_CREAT, 0666, 0);

 pthread_create(&p1, NULL, out, NULL);
 pthread_create(&p2, NULL, in, NULL);
 pthread_join(p1, NULL);
 pthread_join(p2, NULL);

 sem_close(full);
 sem_unlink("full");
 sem_close(empty);
 sem_unlink("empty");

return 0;
}


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include<sys/types.h>

int main(int argc, char *argv[])
{
  pid_t P1,P2,P3,P4;
  P1 = fork(); //创建P1子进程
  if(P1<0)
   perror ("fork");
  else if(P1 == 0)
 {
   printf("进程P2，PID是%d,PPID是%d\n",getpid(),getppid());
   P3 = fork();
   if(P3<0)
    perror ("fork");
   else if(P3 == 0)
 {
   printf("进程P4，PID是%d,PPID是%d\n",getpid(),getppid());
 }

  else if(P3 > 0){
  wait(NULL);
    P4 = fork();
    if(P4<0)
    perror ("fork");
    else if(P4 == 0)
  {
   printf("进程P5，PID是%d,PPID是%d\n",getpid(),getppid());
  }
  else wait(NULL);
 }

 }
 else if(P1 > 0)
 {
   printf("进程P1，PID是%d,PPID是%d\n",getpid(),getppid());wait(NULL);
   P2 = fork();
   if(P2<0)
    perror ("fork");
   else if(P2 == 0)
    printf("进程P3，PID是%d,PPID是%d\n",getpid(),getppid());
  else wait(NULL);

 }
return 0;
}







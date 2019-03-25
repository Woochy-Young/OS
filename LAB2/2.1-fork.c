#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
	pid_t pid;
	pid = fork();

	if( pid < 0 ){	// 没有创建成功
		perror("fork");
	}
	if(0 == pid){ // 子进程
		int ret;
                ret = execl ("/usr/bin/vi", "vi","/home/yang/lab1/os/lab2/test.txt", NULL);
        if (ret == -1)
                perror ("execl");
		sleep(1);  }
        else if(pid > 0){ // 父进程
		while(1){
		sleep(1);
		}
	}
	return 0;
}

#include<stdio.h>
#include<unistd.h>


int main()
{

    pid_t pid;

    asm volatile(
		"movl $0, %%ebx;\n\r" 
	        "movl $0x14, %%eax;\n\r"
		"int  $0x80\n\t" /*����ϵͳ�����ж�*/
                :"=m"(pid)    
            );
    printf("%d\n",pid);

    return 0;
}
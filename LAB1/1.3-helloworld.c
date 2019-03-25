int main()
{
    char* msg = "Hello World\n";
	int len = 12;
	int result = 0;
 
    asm volatile("movl %2, %%edx;\n\r" /*传入参数：要显示的字符串长度*/
             "movl %1, %%ecx;\n\r" /*传入参数：要显示的字符串*/
			 "movl $1, %%ebx;\n\r" /*文件描述符（stdout）*/
			 "movl $4, %%eax;\n\r" /*系统调用号：4 sys_write*/
			 "int  $0x80" /*触发系统调用中断*/
             :"=m"(result)
             :"m"(msg),"r"(len)  /*输入部分：绑定字符串和字符串长度变量*/
             :"%eax"); 
    	
	return 0;
}
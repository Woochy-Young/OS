int main()
{
    char* msg = "Hello World\n";
	int len = 12;
	int result = 0;
 
    asm volatile("movl %2, %%edx;\n\r" /*���������Ҫ��ʾ���ַ�������*/
             "movl %1, %%ecx;\n\r" /*���������Ҫ��ʾ���ַ���*/
			 "movl $1, %%ebx;\n\r" /*�ļ���������stdout��*/
			 "movl $4, %%eax;\n\r" /*ϵͳ���úţ�4 sys_write*/
			 "int  $0x80" /*����ϵͳ�����ж�*/
             :"=m"(result)
             :"m"(msg),"r"(len)  /*���벿�֣����ַ������ַ������ȱ���*/
             :"%eax"); 
    	
	return 0;
}
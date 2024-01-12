#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

void convert_bin(char* mes, int pid);
void send_signal(int b, int pid);

int main(int argc, char* argv[])
{	
	if(argc==3)
	{
		int pid = atoi(argv[1]);
		char* mes = argv[2];
		printf("%i\n",pid);
		printf("%s\n",mes);
		convert_bin(mes,pid);
		return 0;
	}
	else
	{
		printf("Not enough arguments\n");
	}
}


void convert_bin(char* mes, int pid)
{
	int i=0;
	while(mes[i]!='\0')
	{
		
		char hex = mes[i];

		int size = 8;

		while(size>=1)
		{
			size--;
			int b = ((hex & (1<<size))!=0);
			printf("%i",b);
			send_signal(b,pid);
			
		}
		putchar('\n');

		i++;
	}

	send_signal(2,pid);
}


void send_signal(int b,int pid)
{
	if(b==0)
	{
		kill(pid,SIGUSR1);	
	}
	else if(b==1)
	{
		kill(pid,SIGUSR2);
	}
	else if(b==2)
	{
		kill(pid,SIGTERM);
	}


}
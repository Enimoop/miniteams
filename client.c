#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>


void convert_bin(char* mes, int pid);
void send_signal(int b, int pid);
void send_end_text(int pid);
void send_end_pid(int pid);

int main(int argc, char* argv[])
{
	if(argc==3)
	{
		int pid = atoi(argv[1]);
		char cli[7];
		sprintf(cli,"%d",getpid());
		printf("%i\n", getpid());
		char* mes = argv[2];

		if (strlen(mes)>500){
		printf("Ecrivez un message inférieur a 500 caractères\n");
		}
		else{
			printf("%i\n",pid);
			printf("%s\n",mes);
			convert_bin(cli,pid);
			send_end_pid(pid);
			convert_bin(mes,pid);
			send_end_text(pid);
			}
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
			send_signal(b,pid);
		}
	i++;
	}


}

void send_end_text(int pid)
{
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(1,pid);
	send_signal(1,pid);
}

void send_end_pid(int pid)
{
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(0,pid);
	send_signal(1,pid);
	send_signal(0,pid);
	send_signal(0,pid);
}

void send_signal(int b,int pid)
{
	if(b==0)
	{
		kill(pid,SIGUSR1);
		usleep(10000);

	}
	else if(b==1)
	{
		kill(pid,SIGUSR2);
		usleep(10000);

	}


}
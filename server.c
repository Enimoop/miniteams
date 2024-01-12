#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>


void convert_bin_to_char(char* bin_mes)
{
	int i=0;
	int c = strlen(bin_mes);
	while(i<c)
	{
		int b =0;
		int size = 8;

		while(size>=1)
		{
			size--;
			b |= ((bin_mes[i] - '0') << size);
			i++;
		}
		putchar((char)b);
	}
}


void trad_binaire(int x)
{
    static char bin_mes[256];
    static int i = 0;


    if (x == SIGUSR1 || x == SIGUSR2)
    {
        bin_mes[i] = (x == SIGUSR1) ? '0' : '1';
        i++;

        if (i >= 8)
        {
            convert_bin_to_char(bin_mes);
            fflush(stdout);
            i = 0;
        }
    }
    else if (x == SIGTERM)
    {
        putchar('\n');
    }
}


int main(void)
{
	printf("Miniteams starting...\n");
	printf(" My PID is %i\n",getpid());
	printf("Waiting for new messages\n");
	signal(SIGUSR1,trad_binaire);
	signal(SIGUSR2,trad_binaire);
	signal(SIGTERM,trad_binaire);

	while(1)
	{
		usleep(100000);
		pause();
	}
}


	
	

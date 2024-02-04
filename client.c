#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int recu;

void convert_bin(char* mes, int pid);
void send_signal(int b, int pid);
void send_end_text(int pid);
void send_end_pid(int pid);
void recevoir(int x);

int main(int argc, char* argv[]){
    if(argc==3)
    {
        recu = 0;
        int pid = atoi(argv[1]);
        char cli[7];
        sprintf(cli, "%d", getpid());
        printf("%i\n",getpid());
        char* mes = argv[2];

        
        printf("%i\n",pid);       
        printf("%s\n",mes);
        convert_bin(cli,pid);
        send_end_pid(pid);
        recu = 2;
        convert_bin(mes,pid);
        send_end_text(pid);
        
        return 0;
    } else {
        printf("Erreur: nombre d'arguments incorrect\n");
    }
}

void convert_bin(char* mes, int pid){
    int i = 0;
    while(mes[i]!='\0') {
        char hex = mes[i];
        int size = 8;

        while(size>=1) {
           size --;
           int b =((hex & (1<<size))!=0);
           send_signal(b,pid);
        }

        if (recu != 0) {
            signal(SIGUSR1, recevoir);
            while(recu != 2) {
                pause();
            }
            recu = 2;
        }
        i++;
    }
}

void send_end_text(int pid){
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(1,pid);
    send_signal(1,pid);
}

void send_end_pid(int pid){
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(0,pid);
    send_signal(1,pid);
    send_signal(0,pid);
    send_signal(0,pid);
}

void recevoir(int x){
    if(x == SIGUSR1) {
        
        recu = 1;
    }
}

void send_signal(int b, int pid) {
    if (b == 0) {
        usleep(100);
        kill(pid, SIGUSR1);
    } else if (b == 1) {
        usleep(100);
        kill(pid, SIGUSR2);
    }
}

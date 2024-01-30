#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <math.h>


char* message;
int ind;
int pidcli;




char* detect_language(char* message) {

    #define NUM_LANGUAGES 4
    #define NUM_LETTERS 26


    const double avg_frequencies[NUM_LANGUAGES][NUM_LETTERS] = {
    // Français
    {8.13, 0.94, 3.26, 3.67, 17.21, 1.06, 1.27, 0.92, 7.34, 0.31, 0.05, 6.01, 2.96, 7.13, 5.26, 3.01, 0.99, 6.55, 8.08, 7.07, 5.74, 1.32, 0.04, 0.45, 0.3, 0.12},
    // Anglais
    {8.17, 1.49, 2.78, 4.25, 12.7, 2.23, 2.02, 6.09, 6.97, 0.15, 0.77, 4.03, 2.41, 6.75, 7.51, 1.93, 0.09, 6.04, 6.86, 9.37, 3.26, 1.34, 0.06, 1.06, 0.27, 0.05},
    // Allemand
    {6.35, 1.65, 3.03, 5.11, 16.87, 1.49, 3.06, 4.17, 7.27, 0.27, 1.21, 3.44, 2.53, 9.78, 2.59, 0.67, 0.02, 6.89, 6.42, 7.77, 4.11, 0.89, 1.98, 0.04, 0.02, 0.03},
    // Espagnol
    {11.53, 2.22, 4.02, 5.07, 12.61, 0.69, 1.01, 0.7, 6.25, 0.49, 0.01, 4.97, 3.16, 6.71, 8.68, 2.51, 0.88, 6.87, 7.98, 8.67, 4.85, 1.14, 0.08, 0.23, 1.01, 0.47}
};


    int letter_counts[NUM_LETTERS] = {0};
    int total_letters = 0;

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];
        if (isalpha(c)) {
            letter_counts[tolower(c) - 'a']++;
            total_letters++;
        }
    }

    double letter_frequencies[NUM_LETTERS] = {0.0};
    for (int i = 0; i < 26; i++) {
        letter_frequencies[i] = (double)letter_counts[i] / total_letters * 100.0;
    }

    double min_diff = INFINITY;
    int detected_language = -1;

    for (int i = 0; i < 26; i++) {
        double diff = 0.0;
        for (int j = 0; j < 26; j++) {
            diff += fabs(letter_frequencies[j] - avg_frequencies[i][j]);
        }

        if (diff < min_diff) {
            min_diff = diff;
            detected_language = i;
        }
    }

    switch (detected_language) {
        case 0:
            return "French";
        case 1:
            return "English";
        case 2:
            return "German";
        case 3:
            return "Spanish";
        default:
            return "Unknown";
    }
}

char* heure()
{
    static char buffer[80];
    time_t timestamp = time(NULL);

    strftime(buffer, sizeof(buffer),"%X", localtime(&timestamp));
    return buffer;
}

void conversation_log(const char* message)
{
    FILE *fichier;

    fichier = fopen("conversation.log", "at, ccs=UTF-8");

    if (fichier == NULL) {
        fprintf(stderr, "Peut pas ouvrir le fichier avec UTF-8\n");
        return;
    }

    fprintf(fichier,"[%s] %s\n", heure(), message);

    fclose(fichier);
}

void stock_message(char** message, char c)
{

    static int current_size = 1024;

    if(c == 3 || c == 4){
        ind = 0;
        *message = malloc(current_size);
        memset(*message,'\0',current_size);
        return;    
    }

    (*message)[ind] = c;
    ind++;

   


    if (*message == NULL) {
        fprintf(stderr, "Erreur lors du realloc pour message\n");
        exit(EXIT_FAILURE);
    }

   
}


char convert_bin_to_char(char* bin_mes)
{
    int i=0;
    int c = strlen(bin_mes);
    char mes;
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
        mes = b;
   
    }
   
    return mes;
}

void send_signal(int b, int pid) {
    if (b == 1) {
        kill(pid, SIGUSR1);
    }
}

void trad_binaire(int x)
{
    static char bin_mes[9];
    static int i = 0;
 

    if (x == SIGUSR1 || x == SIGUSR2)
    {
       
        bin_mes[i] = (x == SIGUSR1) ? '0' : '1';
        i++;


        if (i >= 8)
        {
            int c = convert_bin_to_char(bin_mes);

            if(pidcli != 0)
            {
                send_signal(pidcli,SIGUSR1);
            }

            if( c == 4)
            {
                printf("[From client %s] ", message);
                pidcli = atoi(message);
                free(message);
               
                   
            }

            if (c == 3)
            {
                conversation_log(message);
                printf("%s",message);
                putchar('\n');
                printf("Language : %s", detect_language(message));
                putchar('\n');
                free(message);
                   
            }


            stock_message(&message, c);
            fflush(stdout);


             i = 0;
           
        }
       
    }
}



int main(void)
{
    ind = 0;
    pidcli = 0;
    message = malloc(1024);
    printf("Miniteams starting...\n");
    printf(" My PID is %i\n",getpid());
    printf("Waiting for new messages\n");


    signal(SIGUSR1,trad_binaire);
    signal(SIGUSR2,trad_binaire);

    while(1)
    {
    usleep(10000);
        pause();
    }
}
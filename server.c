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
    {7.636, 0.901, 3.260, 3.669, 14.715, 1.066, 0.866, 0.737, 7.529, 0.613, 0.074, 5.456, 2.968, 7.095, 5.796, 2.521, 1.362, 6.693, 7.948, 7.244, 6.311, 1.838, 0.049, 0.427, 0.128, 0.326},
    // Anglais
    {8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153, 0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 2.758, 0.978, 2.360, 0.150, 1.974, 0.074},
    // Allemand
    {6.516, 1.886, 3.062, 5.076, 17.396, 1.656, 3.009, 4.577, 7.550, 0.268, 1.217, 3.437, 2.534, 9.776, 2.514, 0.790, 0.018, 7.003, 7.270, 6.154, 4.346, 0.846, 1.891, 0.034, 0.039, 1.134},
    // Espagnol
    {12.525, 1.415, 4.679, 5.810, 13.681, 0.692, 1.008, 0.703, 6.247, 0.493, 0.011, 4.967, 3.157, 6.712, 8.683, 2.510, 0.877, 6.871, 7.977, 4.632, 2.927, 0.898, 0.017, 0.215, 0.898, 0.467}
};


    int letter_counts[NUM_LETTERS] = {0};
    

    for (int i = 0; message[i] != '\0'; i++) {
        char c = message[i];
        if (isalpha(c)) {
            letter_counts[tolower(c) - 'a']++;
            
        }
    }

    double letter_frequencies[NUM_LETTERS] = {0.0};
    for (int i = 0; i < NUM_LETTERS; i++) {
        letter_frequencies[i] = (double)letter_counts[i] / strlen(message) * 100.0;
    }

    double min_diff =100 * NUM_LETTERS;
    int detected_language = -1;

    for (int i = 0; i < NUM_LETTERS; i++) {
        double diff = 0.0;
        for (int j = 0; j < NUM_LANGUAGES; j++) {
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

    if(ind == current_size) {
        current_size *= 2;
        *message = realloc(*message, current_size);
    }


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
        usleep(100);
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
    
        pause();
    }
}
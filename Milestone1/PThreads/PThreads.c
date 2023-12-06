//Zum Kompilieren: gcc PThreads.c -o PThreads 

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_STRING_SIZE 128

char buffer[MAX_STRING_SIZE];
sem_t input_sem, output_sem;

void* input()
{
    while (1)
    {
        printf("Enter Text: ");
        // Speichere Eingabe in Buffer
        fgets(buffer, MAX_STRING_SIZE, stdin);
        // Gib Signal an output thread
        sem_post(&output_sem);
        // Warte auf Signal von output thread
        sem_wait(&input_sem);
    }
    return NULL;
}

void* output()
{
    while (1)
    {
        // Warte auf Signal von input thread
        sem_wait(&output_sem);
        // Gib Eingabe auf der Konsole aus
        printf("Entered Text: %s\n", buffer);
        // Gib Signal an input thread
        sem_post(&input_sem);
    }
    return NULL;
}

int main()
{
    // Threads erstellen
    pthread_t input_thread, output_thread;
    // Semaphoren initialisieren
    sem_init(&input_sem, 0, 0);
    sem_init(&output_sem, 0, 0);
    printf("*Communication test (exit with CTRL+C)\n");

    if (pthread_create(&input_thread, NULL, input, NULL) != 0)
    {
        perror("Error creating input thread.\n");
        return 1;
    }

    if (pthread_create(&output_thread, NULL, output, NULL) != 0)
    {
        perror("Error creating output thread.\n");
    }

    // Warte auf Threads
    pthread_join(input_thread, NULL);
    pthread_join(output_thread, NULL);

    // Semaphoren zerstören
    sem_destroy(&input_sem);
    sem_destroy(&output_sem);

    return 0;
}
// Zum Kompilieren: gcc ProcessPipes.c -o ProcessPipes 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_STRING_SIZE 128

char buffer[MAX_STRING_SIZE];

void input(int In_to_Out[], int Out_to_In[])
{
    close(In_to_Out[0]); // Schließe anderes Ende
    close(Out_to_In[1]); // Schließe anderes Ende
    while (1)
    {
        printf("Enter Text: ");
        memset(buffer, 0, MAX_STRING_SIZE);
        fgets(buffer, MAX_STRING_SIZE, stdin);
        write(In_to_Out[1], buffer, MAX_STRING_SIZE); // Schicke Daten an Ausgabeprozess

        // Antwort vom Server lesen
        long bytesRead = read(Out_to_In[0], buffer, MAX_STRING_SIZE); // Auslesen ob Verarbeitung erfolgreich war
        if (bytesRead <= 0)
        {
            perror("Fehler beim Lesen der Antwort vom Server");
            break;
        }
    }

    close(In_to_Out[1]); // Schließe Pipe
    close(Out_to_In[0]); // Schließe Pipe
}

void output(int In_to_Out[], int Out_to_In[])
{
    close(In_to_Out[1]); // Schließe anderes Ende
    close(Out_to_In[0]); // Schließe anderes Ende
    while (1)
    {
        long bytesRead = read(In_to_Out[0], buffer, MAX_STRING_SIZE); // Lese Pipe
        if (bytesRead <= 0)
        {
            perror("Fehler beim Lesen der Nachricht");
            break;
        }
        printf("Entered Text: %s\n", buffer);

        // Eingabeprozess die Verarbeitung mitteilen
        const char *response = "Processing successful";
        write(Out_to_In[1], response, strlen(response) + 1); // String + Nullterminator
    }

    close(In_to_Out[0]); // Schließe Pipe
    close(Out_to_In[1]); // Schließe Pipe
}

int main(int argc, char **argv)
{
    printf("*Communication test (exit with CTRL+C)\n");

    int In_to_Out[2], Out_to_In[2];                     // Pipes File Descriptor //zwei da uni-direktional
    if (pipe(In_to_Out) == -1 || pipe(Out_to_In) == -1) // Check ob Erstellen fehlgeschlagen ist
    {
        perror("Pipe creation failed");
        return 1;
    }

    int pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return 1;
    }

    if (pid == 0)
    { // Kindprozess (Eingabe)
        input(In_to_Out, Out_to_In);
        exit(EXIT_SUCCESS);
    }
    else
    { // Elternprozess (Ausgabe)
        output(In_to_Out, Out_to_In);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

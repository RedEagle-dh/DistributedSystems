//Zum Kompilieren: gcc ProcessSockets.c -o ProcessSockets 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define MAX_STRING_SIZE 128

char buffer[MAX_STRING_SIZE];

void input(int fd[])
{
    close(fd[0]); // Schließe anderes Ende
    while (1)
    {
        printf("Enter Text: ");
        memset(buffer, 0, MAX_STRING_SIZE);
        fgets(buffer, MAX_STRING_SIZE, stdin);
        write(fd[1], buffer, MAX_STRING_SIZE); // Schicke Daten an den Socket (Ausgabeprozess)

        // Antwort vom Server lesen
        long bytesRead = read(fd[1], buffer, MAX_STRING_SIZE); //Auslesen ob Verarbeitung erfolgreich war
        if (bytesRead <= 0)
        {
            perror("Fehler beim Lesen der Antwort vom Server");
            break;
        }
    }

    close(fd[1]); // Schließe den Socket
}

void output(int fd[])
{
    close(fd[1]); // Schließe anderes Ende
    while (1)
    {
        long bytesRead = read(fd[0], buffer, MAX_STRING_SIZE); // Lese Socket
        if (bytesRead <= 0)
        {
            perror("Fehler beim Lesen der Nachricht");
            break;
        }
        printf("Entered Text: %s\n", buffer);

        // Eingabeprozess Verarbeitung mitteilen
        const char *response = "Processing successful";
        write(fd[0], response, strlen(response) + 1); // String + Nullterminator
    }

    close(fd[0]); // Schließe den Socket
}

int main(int argc, char **argv)
{
    printf("*Communication test (exit with CTRL+C)\n");

    int fd[2];                                         // Socketpair File Descriptor
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) == -1) // Check ob Erstellen fehlgeschlagen ist
    {
        perror("Socket creation failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return 1;
    }

    if (pid == 0)
    { // Kindprozess (Eingabe)
        input(fd);
        exit(EXIT_SUCCESS);
    }
    else
    { // Elternprozess (Ausgabe)
        output(fd);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

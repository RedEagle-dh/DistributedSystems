// Zum Kompilieren: gcc ProcessFile.c -o ProcessFile 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define FILE_NAME "change.txt"
#define MAX_STRING_SIZE 128

char buffer[MAX_STRING_SIZE];

struct inotify_event *event;

// Liest den Inhalt der Datei und gibt ihn auf der Konsole aus.
void print_file_content()
{
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char ch;
    printf("Entered text: ");
    while ((ch = fgetc(file)) != EOF)
    {
        putchar(ch);
    }
    putchar('\n');
    fclose(file);
}


void output()
{
    int fd, wd; // File descriptor, Watch descriptor
    // temporärer Buffer, um die Events zu lesen
    char buff[EVENT_SIZE];
    memset(buff, 0, EVENT_SIZE);

    // Inotify initialisieren
    fd = inotify_init();
    if (fd == -1)
    {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Watch erstellen für IN_CLOSE_WRITE (File closed after being opened in write-only mode)
    wd = inotify_add_watch(fd, FILE_NAME, IN_CLOSE_WRITE);
    if (wd == -1)
    {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // read() blockiert, bis ein Event eintritt
        // read schreibt das Event in den Buffer
        int length = read(fd, buff, EVENT_SIZE);
        if (length == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
            // Event auslesen, typecast auf inotify_event
            event = (struct inotify_event *) buff;

            // Wenn das event IN_CLOSE_WRITE ist, dann print_file_content()
            if (event->mask & IN_CLOSE_WRITE)
            {
                print_file_content();
            }
    }
    
    // Entferne Event watch und schließe File descriptor
    inotify_rm_watch(fd, wd);
    close(fd);
}

void input()
{
    // temporärer Buffer, um die Events zu lesen
    char buff[EVENT_SIZE];
    memset(buff, 0, EVENT_SIZE);

    int fd, wd;
    fd = inotify_init();
    if (fd == -1)
    {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Watch erstellen für IN_ACCESS (File accessed)
    wd = inotify_add_watch(fd, FILE_NAME, IN_ACCESS);
    if (wd == -1)
    {
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Text einlesen und in die Datei schreiben
        printf("Enter text: ");
        fgets(buffer, MAX_STRING_SIZE, stdin);

        FILE *file = fopen(FILE_NAME, "w");
        if (file == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        fprintf(file, "%s", buffer);
        fclose(file);

        // Read() blockiert, bis ein Event eintritt
        // Wenn file durch forked process gelesen wurde, dann mache weiter. 
        int length = read(fd, buff, EVENT_SIZE);
        if (length == -1)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

    }
    // Entferne Event watch und schließe File descriptor
    inotify_rm_watch(fd, wd);
    close(fd);
}

int main()
{
    printf("*Communication test (exit with CTRL+C)\n");
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        output();
        exit(0);
    }
    else
    {
        input();
    }

    return 0;
}

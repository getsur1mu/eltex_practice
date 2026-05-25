#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_FILE "numbers.txt"
#define LINE_SIZE 512

typedef struct SharedData
{
    sem_t semaphore;
    long position;
    int finished;
} SharedData;

void WriteRandomLine(FILE *file)
{
    int count = 3 + rand() % 8;

    for (int i = 0; i < count; i++)
    {
        int value = rand() % 200 - 100;
        fprintf(file, "%d", value);

        if (i + 1 < count)
            fprintf(file, " ");
    }

    fprintf(file, "\n");
}

void AnalyzeLine(const char *line)
{
    char copy[LINE_SIZE];
    char *token;
    int min = 0;
    int max = 0;
    int first = 1;

    strncpy(copy, line, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';

    token = strtok(copy, " \t\n");
    while (token != NULL)
    {
        int value = atoi(token);

        if (first)
        {
            min = value;
            max = value;
            first = 0;
        }
        else
        {
            if (value < min)
                min = value;
            if (value > max)
                max = value;
        }

        token = strtok(NULL, " \t\n");
    }

    if (!first)
    {
        printf("Дочерний процесс: %s", line);
        printf("min = %d, max = %d\n", min, max);
    }
}

void ChildWork(const char *fileName, SharedData *shared)
{
    int work = 1;

    while (work)
    {
        FILE *file;
        char line[LINE_SIZE];
        int hasLine = 0;

        sem_wait(&shared->semaphore);

        file = fopen(fileName, "r");
        if (file != NULL)
        {
            fseek(file, shared->position, SEEK_SET);

            if (fgets(line, sizeof(line), file) != NULL)
            {
                shared->position = ftell(file);
                hasLine = 1;
            }

            fclose(file);
        }

        if (!hasLine && shared->finished)
            work = 0;

        sem_post(&shared->semaphore);

        if (hasLine)
            AnalyzeLine(line);
        else if (work)
            sleep(1);
    }
}

void ParentWork(const char *fileName, int lines, SharedData *shared)
{
    srand(time(NULL) ^ getpid());

    for (int i = 0; i < lines; i++)
    {
        FILE *file;

        sem_wait(&shared->semaphore);

        file = fopen(fileName, "a");
        if (file == NULL)
        {
            perror(fileName);
            shared->finished = 1;
            sem_post(&shared->semaphore);
            return;
        }

        WriteRandomLine(file);
        fclose(file);

        sem_post(&shared->semaphore);

        printf("Родительский процесс записал строку %d\n", i + 1);
        sleep(1);
    }

    sem_wait(&shared->semaphore);
    shared->finished = 1;
    sem_post(&shared->semaphore);
}

int main(int argc, char *argv[])
{
    const char *fileName = DEFAULT_FILE;
    int lines = 20;
    SharedData *shared;
    pid_t pid;
    FILE *file;

    if (argc > 1)
        fileName = argv[1];
    if (argc > 2)
        lines = atoi(argv[2]);
    if (lines <= 0)
        lines = 20;

    file = fopen(fileName, "w");
    if (file == NULL)
    {
        perror(fileName);
        return 1;
    }
    fclose(file);

    shared = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE,
                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    shared->position = 0;
    shared->finished = 0;

    if (sem_init(&shared->semaphore, 1, 1) == -1)
    {
        perror("sem_init");
        munmap(shared, sizeof(SharedData));
        return 1;
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        sem_destroy(&shared->semaphore);
        munmap(shared, sizeof(SharedData));
        return 1;
    }

    if (pid == 0)
    {
        ChildWork(fileName, shared);
        munmap(shared, sizeof(SharedData));
        return 0;
    }

    ParentWork(fileName, lines, shared);
    waitpid(pid, NULL, 0);

    sem_destroy(&shared->semaphore);
    munmap(shared, sizeof(SharedData));

    return 0;
}

#include "common.h"

#include <time.h>

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

int main(int argc, char *argv[])
{
    const char *fileName = DEFAULT_FILE;
    int lines = 20;
    int semid;

    if (argc > 1)
        fileName = argv[1];
    if (argc > 2)
        lines = atoi(argv[2]);
    if (lines <= 0)
        lines = 20;

    semid = GetSemaphore(fileName);
    if (semid == -1)
        return 1;

    srand(time(NULL) ^ getpid());

    for (int i = 0; i < lines; i++)
    {
        FILE *file;

        if (LockSemaphore(semid) != 0)
            return 1;

        file = fopen(fileName, "a");
        if (file == NULL)
        {
            perror(fileName);
            UnlockSemaphore(semid);
            return 1;
        }

        WriteRandomLine(file);
        fclose(file);
        UnlockSemaphore(semid);

        printf("Производитель записал строку %d\n", i + 1);
        sleep(1);
    }

    return 0;
}

#include "common.h"

#include <signal.h>

volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

long ReadPosition(const char *positionFile)
{
    FILE *file = fopen(positionFile, "r");
    long position = 0;

    if (file == NULL)
        return 0;

    fscanf(file, "%ld", &position);
    fclose(file);

    return position;
}

void WritePosition(const char *positionFile, long position)
{
    FILE *file = fopen(positionFile, "w");

    if (file == NULL)
    {
        perror(positionFile);
        return;
    }

    fprintf(file, "%ld\n", position);
    fclose(file);
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
        printf("Строка: %s", line);
        printf("min = %d, max = %d\n", min, max);
    }
}

int main(int argc, char *argv[])
{
    const char *fileName = DEFAULT_FILE;
    char positionFile[256];
    int semid;

    if (argc > 1)
        fileName = argv[1];

    signal(SIGINT, SignalHandler);

    semid = GetSemaphore(fileName);
    if (semid == -1)
        return 1;

    GetPositionFileName(fileName, positionFile, sizeof(positionFile));
    printf("Потребитель запущен. Для остановки нажмите Ctrl+C.\n");

    while (work)
    {
        FILE *file;
        char line[LINE_SIZE];
        long position;
        int hasLine = 0;

        if (LockSemaphore(semid) != 0)
            return 1;

        file = fopen(fileName, "r");
        if (file == NULL)
        {
            perror(fileName);
            UnlockSemaphore(semid);
            sleep(1);
            continue;
        }

        position = ReadPosition(positionFile);
        fseek(file, position, SEEK_SET);

        if (fgets(line, sizeof(line), file) != NULL)
        {
            position = ftell(file);
            WritePosition(positionFile, position);
            hasLine = 1;
        }

        fclose(file);
        UnlockSemaphore(semid);

        if (hasLine)
            AnalyzeLine(line);
        else
            sleep(1);
    }

    return 0;
}

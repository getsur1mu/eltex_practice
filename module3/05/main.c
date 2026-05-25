#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t sigintCount = 0;
volatile sig_atomic_t sigquitCount = 0;

void SignalHandler(int signalNumber)
{
    if (signalNumber == SIGINT)
        sigintCount++;
    else if (signalNumber == SIGQUIT)
        sigquitCount++;
}

void WriteSignalMessages(FILE *file, int *printedInt, int *printedQuit)
{
    while (*printedInt < sigintCount)
    {
        fprintf(file, "Получен и обработан сигнал SIGINT\n");
        (*printedInt)++;
    }

    while (*printedQuit < sigquitCount)
    {
        fprintf(file, "Получен и обработан сигнал SIGQUIT\n");
        (*printedQuit)++;
    }

    fflush(file);
}

int main(int argc, char *argv[])
{
    const char *fileName = "counter.log";
    FILE *file;
    struct sigaction action;
    int counter = 1;
    int printedInt = 0;
    int printedQuit = 0;

    if (argc > 1)
        fileName = argv[1];

    file = fopen(fileName, "a");
    if (file == NULL)
    {
        perror(fileName);
        return 1;
    }

    memset(&action, 0, sizeof(action));
    action.sa_handler = SignalHandler;
    sigemptyset(&action.sa_mask);

    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        perror("sigaction SIGINT");
        fclose(file);
        return 1;
    }

    if (sigaction(SIGQUIT, &action, NULL) == -1)
    {
        perror("sigaction SIGQUIT");
        fclose(file);
        return 1;
    }

    printf("PID процесса: %d\n", getpid());
    printf("Данные пишутся в файл: %s\n", fileName);
    printf("Программа завершится после третьего SIGINT.\n");

    while (sigintCount < 3)
    {
        sleep(1);
        WriteSignalMessages(file, &printedInt, &printedQuit);

        if (sigintCount >= 3)
            break;

        fprintf(file, "%d\n", counter);
        fflush(file);
        counter++;
    }

    WriteSignalMessages(file, &printedInt, &printedQuit);
    fprintf(file, "Программа завершилась после третьего SIGINT\n");
    fclose(file);

    return 0;
}

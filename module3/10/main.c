#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHM_KEY 0x101010
#define MAX_NUMBERS 32

#define STATE_EMPTY 0
#define STATE_DATA_READY 1
#define STATE_RESULT_READY 2
#define STATE_STOP 3

typedef struct SharedData
{
    volatile int state;
    int count;
    int numbers[MAX_NUMBERS];
    int min;
    int max;
} SharedData;

volatile sig_atomic_t stopWork = 0;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    stopWork = 1;
}

void GenerateNumbers(SharedData *shared)
{
    shared->count = 5 + rand() % 10;

    for (int i = 0; i < shared->count; i++)
    {
        shared->numbers[i] = rand() % 200 - 100;
    }
}

void PrintNumbers(SharedData *shared)
{
    printf("Набор: ");
    for (int i = 0; i < shared->count; i++)
    {
        printf("%d", shared->numbers[i]);
        if (i + 1 < shared->count)
            printf(" ");
    }
    printf(" -> min = %d, max = %d\n", shared->min, shared->max);
}

void ChildWork(SharedData *shared)
{
    signal(SIGINT, SIG_IGN);

    while (1)
    {
        if (shared->state == STATE_DATA_READY)
        {
            int min = shared->numbers[0];
            int max = shared->numbers[0];

            for (int i = 1; i < shared->count; i++)
            {
                if (shared->numbers[i] < min)
                    min = shared->numbers[i];
                if (shared->numbers[i] > max)
                    max = shared->numbers[i];
            }

            shared->min = min;
            shared->max = max;
            shared->state = STATE_RESULT_READY;
        }
        else if (shared->state == STATE_STOP)
        {
            break;
        }
        else
        {
            usleep(10000);
        }
    }
}

int main()
{
    int shmId;
    SharedData *shared;
    pid_t pid;
    int processed = 0;

    signal(SIGINT, SignalHandler);

    shmId = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmId == -1)
    {
        perror("shmget");
        return 1;
    }

    shared = shmat(shmId, NULL, 0);
    if (shared == (void *)-1)
    {
        perror("shmat");
        shmctl(shmId, IPC_RMID, NULL);
        return 1;
    }

    memset(shared, 0, sizeof(SharedData));
    srand(time(NULL) ^ getpid());

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        shmdt(shared);
        shmctl(shmId, IPC_RMID, NULL);
        return 1;
    }

    if (pid == 0)
    {
        ChildWork(shared);
        shmdt(shared);
        return 0;
    }

    printf("Программа работает до SIGINT. Нажмите Ctrl+C для остановки.\n");

    while (!stopWork)
    {
        while (shared->state != STATE_EMPTY && !stopWork)
            usleep(10000);

        if (stopWork)
            break;

        GenerateNumbers(shared);
        shared->state = STATE_DATA_READY;

        while (shared->state != STATE_RESULT_READY && !stopWork)
            usleep(10000);

        if (shared->state == STATE_RESULT_READY)
        {
            PrintNumbers(shared);
            processed++;
            shared->state = STATE_EMPTY;
        }

        sleep(1);
    }

    shared->state = STATE_STOP;
    waitpid(pid, NULL, 0);

    printf("\nОбработано наборов данных: %d\n", processed);

    shmdt(shared);
    shmctl(shmId, IPC_RMID, NULL);

    return 0;
}

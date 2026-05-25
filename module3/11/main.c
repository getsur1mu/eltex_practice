#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHM_NAME "/eltex_m3_11_shm"
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
    int fd;
    SharedData *shared;
    pid_t pid;
    int processed = 0;

    signal(SIGINT, SignalHandler);

    shm_unlink(SHM_NAME);
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(fd, sizeof(SharedData)) == -1)
    {
        perror("ftruncate");
        close(fd);
        shm_unlink(SHM_NAME);
        return 1;
    }

    shared = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE,
                  MAP_SHARED, fd, 0);
    close(fd);

    if (shared == MAP_FAILED)
    {
        perror("mmap");
        shm_unlink(SHM_NAME);
        return 1;
    }

    memset(shared, 0, sizeof(SharedData));
    srand(time(NULL) ^ getpid());

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        munmap(shared, sizeof(SharedData));
        shm_unlink(SHM_NAME);
        return 1;
    }

    if (pid == 0)
    {
        ChildWork(shared);
        munmap(shared, sizeof(SharedData));
        return 0;
    }

    printf("POSIX shared memory. Для остановки нажмите Ctrl+C.\n");

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

    munmap(shared, sizeof(SharedData));
    shm_unlink(SHM_NAME);

    return 0;
}

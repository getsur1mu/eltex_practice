#ifndef COMMON_H
#define COMMON_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_FILE "numbers.txt"
#define LINE_SIZE 512

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int EnsureFile(const char *fileName)
{
    int fd = open(fileName, O_CREAT | O_RDWR, 0666);

    if (fd == -1)
    {
        perror(fileName);
        return -1;
    }

    close(fd);
    return 0;
}

int GetSemaphore(const char *fileName)
{
    key_t key;
    int semid;
    union semun arg;

    if (EnsureFile(fileName) != 0)
        return -1;

    key = ftok(fileName, 'S');
    if (key == -1)
    {
        perror("ftok");
        return -1;
    }

    semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid != -1)
    {
        arg.val = 1;
        if (semctl(semid, 0, SETVAL, arg) == -1)
        {
            perror("semctl");
            return -1;
        }
        return semid;
    }

    if (errno != EEXIST)
    {
        perror("semget");
        return -1;
    }

    semid = semget(key, 1, 0666);
    if (semid == -1)
        perror("semget");

    return semid;
}

int LockSemaphore(int semid)
{
    struct sembuf operation = {0, -1, 0};

    if (semop(semid, &operation, 1) == -1)
    {
        perror("semop lock");
        return -1;
    }

    return 0;
}

int UnlockSemaphore(int semid)
{
    struct sembuf operation = {0, 1, 0};

    if (semop(semid, &operation, 1) == -1)
    {
        perror("semop unlock");
        return -1;
    }

    return 0;
}

void GetPositionFileName(const char *fileName, char *positionFile, size_t size)
{
    snprintf(positionFile, size, "%s.pos", fileName);
}

#endif

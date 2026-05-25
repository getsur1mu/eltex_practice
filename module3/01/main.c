#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int IsNumber(const char *str, double *number)
{
    char *endptr;

    if (str[0] == '\0')
        return 0;

    errno = 0;
    *number = strtod(str, &endptr);

    if (endptr == str || *endptr != '\0' || errno == ERANGE)
        return 0;

    return 1;
}

void ProcessArguments(char *argv[], int start, int end, const char *processName)
{
    double number;

    for (int i = start; i < end; i++)
    {
        printf("[%s pid=%d] ", processName, getpid());

        if (IsNumber(argv[i], &number))
        {
            printf("%s -> %g * 2 = %g\n", argv[i], number, number * 2);
        }
        else
        {
            printf("%s\n", argv[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int status;
    int total;
    int parentCount;

    if (argc < 2)
    {
        printf("Использование:\n");
        printf("./arguments <аргументы>\n");
        printf("Пример:\n");
        printf("./arguments 10 text 3.14 hello -7\n");
        return 0;
    }

    total = argc - 1;
    parentCount = (total + 1) / 2;

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        ProcessArguments(argv, 1 + parentCount, argc, "Дочерний процесс");
        return 0;
    }

    ProcessArguments(argv, 1, 1 + parentCount, "Родительский процесс");
    waitpid(pid, &status, 0);

    return 0;
}

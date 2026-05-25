#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int HasSlash(const char *str)
{
    while (*str != '\0')
    {
        if (*str == '/')
            return 1;
        str++;
    }

    return 0;
}

void ExecProgram(char *args[])
{
    char localPath[MAX_LINE];

    execvp(args[0], args);

    if (!HasSlash(args[0]))
    {
        snprintf(localPath, sizeof(localPath), "./%s", args[0]);
        execv(localPath, args);
    }

    if (errno == ENOENT)
        fprintf(stderr, "Программа не найдена: %s\n", args[0]);
    else
        perror(args[0]);

    exit(1);
}

int ParseCommand(char *line, char *args[])
{
    int argc = 0;
    char *token = strtok(line, " \t\n");

    while (token != NULL && argc < MAX_ARGS - 1)
    {
        args[argc] = token;
        argc++;
        token = strtok(NULL, " \t\n");
    }

    args[argc] = NULL;
    return argc;
}

void RunCommand(char *args[])
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        ExecProgram(args);
    }

    waitpid(pid, &status, 0);
}

void ChangeDirectory(char *args[])
{
    const char *path = args[1];

    if (args[2] != NULL)
    {
        printf("cd: слишком много аргументов.\n");
        return;
    }

    if (path == NULL)
    {
        path = getenv("HOME");
        if (path == NULL)
        {
            printf("cd: переменная HOME не задана.\n");
            return;
        }
    }

    if (chdir(path) == -1)
    {
        perror("cd");
    }
}

int main()
{
    char line[MAX_LINE];
    char *args[MAX_ARGS];
    int argc;

    printf("Мини командный интерпретатор. Для выхода введите exit.\n");

    while (1)
    {
        printf("mini-shell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        argc = ParseCommand(line, args);
        if (argc == 0)
            continue;

        if (strcmp(args[0], "exit") == 0)
            break;

        if (strcmp(args[0], "cd") == 0)
        {
            ChangeDirectory(args);
            continue;
        }

        RunCommand(args);
    }

    return 0;
}

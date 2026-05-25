#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

typedef struct Command
{
    char *argv[MAX_ARGS];
    int argc;
    char *inputFile;
    char *outputFile;
    int append;
} Command;

void InitCommand(Command *command)
{
    memset(command, 0, sizeof(Command));
}

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

int AddCommand(Command **commands, int *count, int *capacity)
{
    Command *temp;

    if (*count >= *capacity)
    {
        *capacity *= 2;
        temp = realloc(*commands, sizeof(Command) * (*capacity));
        if (temp == NULL)
        {
            perror("realloc");
            return -1;
        }
        *commands = temp;
    }

    InitCommand(&(*commands)[*count]);
    (*count)++;
    return 0;
}

int ParseLine(char *line, Command **commands, int *commandCount)
{
    int capacity = 4;
    Command *current;
    char *token;

    *commands = calloc(capacity, sizeof(Command));
    if (*commands == NULL)
    {
        perror("calloc");
        return -1;
    }

    *commandCount = 0;
    if (AddCommand(commands, commandCount, &capacity) != 0)
        return -1;

    current = &(*commands)[*commandCount - 1];
    token = strtok(line, " \t\n");

    while (token != NULL)
    {
        if (strcmp(token, "|") == 0)
        {
            if (current->argc == 0)
            {
                printf("Ошибка: пустая команда в конвейере.\n");
                return -1;
            }

            if (AddCommand(commands, commandCount, &capacity) != 0)
                return -1;
            current = &(*commands)[*commandCount - 1];
        }
        else if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " \t\n");
            if (token == NULL)
            {
                printf("Ошибка: не указан файл для ввода.\n");
                return -1;
            }
            current->inputFile = token;
        }
        else if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0)
        {
            int append = (strcmp(token, ">>") == 0);

            token = strtok(NULL, " \t\n");
            if (token == NULL)
            {
                printf("Ошибка: не указан файл для вывода.\n");
                return -1;
            }
            current->outputFile = token;
            current->append = append;
        }
        else
        {
            if (current->argc >= MAX_ARGS - 1)
            {
                printf("Ошибка: слишком много аргументов.\n");
                return -1;
            }
            current->argv[current->argc] = token;
            current->argc++;
            current->argv[current->argc] = NULL;
        }

        token = strtok(NULL, " \t\n");
    }

    if ((*commands)[*commandCount - 1].argc == 0)
    {
        printf("Ошибка: пустая команда.\n");
        return -1;
    }

    return 0;
}

void OpenInputFile(const char *name)
{
    int fd = open(name, O_RDONLY);

    if (fd == -1)
    {
        perror(name);
        exit(1);
    }

    dup2(fd, STDIN_FILENO);
    close(fd);
}

void OpenOutputFile(const char *name, int append)
{
    int flags = O_WRONLY | O_CREAT;
    int fd;

    if (append)
        flags |= O_APPEND;
    else
        flags |= O_TRUNC;

    fd = open(name, flags, 0644);
    if (fd == -1)
    {
        perror(name);
        exit(1);
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);
}

void CloseIfNeeded(int fd)
{
    if (fd != -1)
        close(fd);
}

void ExecutePipeline(Command *commands, int commandCount)
{
    int previousPipe = -1;
    int started = 0;
    pid_t *pids = malloc(sizeof(pid_t) * commandCount);

    if (pids == NULL)
    {
        perror("malloc");
        return;
    }

    for (int i = 0; i < commandCount; i++)
    {
        int pipefd[2] = {-1, -1};
        pid_t pid;

        if (i + 1 < commandCount)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                break;
            }
        }

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            CloseIfNeeded(pipefd[0]);
            CloseIfNeeded(pipefd[1]);
            break;
        }

        if (pid == 0)
        {
            if (previousPipe != -1)
                dup2(previousPipe, STDIN_FILENO);

            if (pipefd[1] != -1)
                dup2(pipefd[1], STDOUT_FILENO);

            CloseIfNeeded(previousPipe);
            CloseIfNeeded(pipefd[0]);
            CloseIfNeeded(pipefd[1]);

            if (commands[i].inputFile != NULL)
                OpenInputFile(commands[i].inputFile);

            if (commands[i].outputFile != NULL)
                OpenOutputFile(commands[i].outputFile, commands[i].append);

            ExecProgram(commands[i].argv);
        }

        pids[i] = pid;
        started++;
        CloseIfNeeded(previousPipe);
        CloseIfNeeded(pipefd[1]);
        previousPipe = pipefd[0];
    }

    CloseIfNeeded(previousPipe);

    for (int i = 0; i < started; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
}

int main()
{
    char line[MAX_LINE];
    Command *commands = NULL;
    int commandCount = 0;

    printf("Мини shell с каналами и перенаправлением. Для выхода введите exit.\n");

    while (1)
    {
        printf("pipe-shell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");
            break;
        }

        if (strcmp(line, "\n") == 0)
            continue;

        if (ParseLine(line, &commands, &commandCount) != 0)
        {
            free(commands);
            commands = NULL;
            continue;
        }

        if (commandCount == 1 && strcmp(commands[0].argv[0], "exit") == 0)
        {
            free(commands);
            break;
        }

        ExecutePipeline(commands, commandCount);
        free(commands);
        commands = NULL;
    }

    return 0;
}

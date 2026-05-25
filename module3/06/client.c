#include "common.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

void SendMessage(int queue, int clientType, const char *text)
{
    Message message;

    message.mtype = SERVER_TYPE;
    message.sender = clientType;
    strncpy(message.text, text, MAX_TEXT - 1);
    message.text[MAX_TEXT - 1] = '\0';

    if (msgsnd(queue, &message, sizeof(Message) - sizeof(long), 0) == -1)
    {
        perror("msgsnd");
    }
}

void ReceiveMessages(int queue, int clientType)
{
    Message message;

    while (work)
    {
        ssize_t result = msgrcv(queue, &message, sizeof(Message) - sizeof(long), clientType, 0);

        if (result == -1)
        {
            if (errno == EINTR)
                continue;
            perror("msgrcv");
            break;
        }

        printf("\n[%d] %s\n> ", message.sender, message.text);
        fflush(stdout);
    }
}

int main(int argc, char *argv[])
{
    int queue;
    int clientType;
    char line[MAX_TEXT];
    pid_t child;

    if (argc != 2)
    {
        printf("Использование:\n");
        printf("./client <тип клиента: 20, 30, 40...>\n");
        return 0;
    }

    clientType = atoi(argv[1]);
    if (clientType <= SERVER_TYPE)
    {
        printf("Тип клиента должен быть больше %d, например 20 или 30.\n", SERVER_TYPE);
        return 0;
    }

    signal(SIGINT, SignalHandler);

    queue = msgget(QUEUE_KEY, 0666);
    if (queue == -1)
    {
        perror("msgget");
        printf("Сначала запустите сервер.\n");
        return 1;
    }

    SendMessage(queue, clientType, "connect");

    child = fork();
    if (child < 0)
    {
        perror("fork");
        return 1;
    }

    if (child == 0)
    {
        ReceiveMessages(queue, clientType);
        return 0;
    }

    printf("Клиент %d запущен. Для выхода введите shutdown.\n", clientType);
    while (work)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0)
            continue;

        SendMessage(queue, clientType, line);

        if (strcmp(line, "shutdown") == 0)
            break;
    }

    work = 0;
    kill(child, SIGINT);
    waitpid(child, NULL, 0);

    return 0;
}

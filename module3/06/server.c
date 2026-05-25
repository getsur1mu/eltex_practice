#include "common.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

int queueId = -1;
volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

int FindClient(int *clients, int count, int clientType)
{
    for (int i = 0; i < count; i++)
    {
        if (clients[i] == clientType)
            return i;
    }

    return -1;
}

void AddClient(int *clients, int *count, int clientType)
{
    if (FindClient(clients, *count, clientType) != -1)
        return;

    if (*count >= MAX_CLIENTS)
    {
        printf("Слишком много клиентов, новый клиент не добавлен.\n");
        return;
    }

    clients[*count] = clientType;
    (*count)++;
    printf("Клиент %d подключен.\n", clientType);
}

void RemoveClient(int *clients, int *count, int clientType)
{
    int index = FindClient(clients, *count, clientType);

    if (index == -1)
        return;

    for (int i = index; i < *count - 1; i++)
    {
        clients[i] = clients[i + 1];
    }

    (*count)--;
    printf("Клиент %d отключен.\n", clientType);
}

void SendToClients(int queue, int *clients, int count, Message *message)
{
    Message out;

    for (int i = 0; i < count; i++)
    {
        if (clients[i] == message->sender)
            continue;

        out.mtype = clients[i];
        out.sender = message->sender;
        strncpy(out.text, message->text, MAX_TEXT - 1);
        out.text[MAX_TEXT - 1] = '\0';

        if (msgsnd(queue, &out, sizeof(Message) - sizeof(long), 0) == -1)
        {
            perror("msgsnd");
        }
    }
}

int main()
{
    Message message;
    int clients[MAX_CLIENTS];
    int clientCount = 0;

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    queueId = msgget(QUEUE_KEY, IPC_CREAT | 0666);
    if (queueId == -1)
    {
        perror("msgget");
        return 1;
    }

    printf("Сервер запущен. Очередь: %d\n", queueId);
    printf("Для остановки сервера нажмите Ctrl+C.\n");

    while (work)
    {
        ssize_t result = msgrcv(queueId, &message, sizeof(Message) - sizeof(long), SERVER_TYPE, 0);

        if (result == -1)
        {
            if (!work)
                break;
            perror("msgrcv");
            continue;
        }

        AddClient(clients, &clientCount, message.sender);

        if (strcmp(message.text, "connect") == 0)
            continue;

        if (strcmp(message.text, "shutdown") == 0)
        {
            RemoveClient(clients, &clientCount, message.sender);
            continue;
        }

        printf("От клиента %d: %s\n", message.sender, message.text);
        SendToClients(queueId, clients, clientCount, &message);
    }

    if (queueId != -1)
    {
        msgctl(queueId, IPC_RMID, NULL);
    }

    printf("\nСервер завершен, очередь удалена.\n");
    return 0;
}

#include "common.h"

#include <errno.h>
#include <signal.h>
#include <sys/time.h>

typedef struct ClientNode
{
    char ip[INET_ADDRSTRLEN];
    unsigned short port;
    int counter;
    struct ClientNode *next;
} ClientNode;

volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

ClientNode *FindClient(ClientNode *head, const char *ip, unsigned short port)
{
    ClientNode *current = head;

    while (current != NULL)
    {
        if (current->port == port && strcmp(current->ip, ip) == 0)
            return current;

        current = current->next;
    }

    return NULL;
}

ClientNode *GetOrCreateClient(ClientNode **head, const char *ip, unsigned short port)
{
    ClientNode *client = FindClient(*head, ip, port);

    if (client != NULL)
        return client;

    client = malloc(sizeof(ClientNode));
    if (client == NULL)
        return NULL;

    strncpy(client->ip, ip, sizeof(client->ip) - 1);
    client->ip[sizeof(client->ip) - 1] = '\0';
    client->port = port;
    client->counter = 0;
    client->next = *head;
    *head = client;

    printf("Новый клиент %s:%d\n", ip, port);
    return client;
}

void RemoveClient(ClientNode **head, const char *ip, unsigned short port)
{
    ClientNode *current = *head;
    ClientNode *previous = NULL;

    while (current != NULL)
    {
        if (current->port == port && strcmp(current->ip, ip) == 0)
        {
            if (previous == NULL)
                *head = current->next;
            else
                previous->next = current->next;

            printf("Клиент %s:%d закрыт, счетчик сброшен.\n", ip, port);
            free(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}

void FreeClients(ClientNode *head)
{
    while (head != NULL)
    {
        ClientNode *next = head->next;
        free(head);
        head = next;
    }
}

int main(int argc, char *argv[])
{
    const char *serverIp = DEFAULT_SERVER_IP;
    unsigned short serverPort = DEFAULT_SERVER_PORT;
    int socketFd;
    ClientNode *clients = NULL;
    struct timeval timeout;

    if (argc > 1)
        serverIp = argv[1];
    if (argc > 2)
        serverPort = (unsigned short)atoi(argv[2]);

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    srand(time(NULL) ^ getpid());
    setvbuf(stdout, NULL, _IONBF, 0);

    socketFd = CreateRawSocket();
    if (socketFd == -1)
        return 1;

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    printf("RAW echo-server запущен на %s:%d\n", serverIp, serverPort);
    printf("Для остановки нажмите Ctrl+C.\n");

    while (work)
    {
        char clientIp[INET_ADDRSTRLEN];
        unsigned short clientPort = 0;
        char text[MAX_TEXT];
        int result = ReadRawUdp(socketFd, serverPort, 0,
                                clientIp, sizeof(clientIp),
                                &clientPort, text, sizeof(text));

        if (result == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                continue;

            perror("recv");
            continue;
        }

        if (result == 0)
            continue;

        if (strcmp(text, CLOSE_MESSAGE) == 0)
        {
            RemoveClient(&clients, clientIp, clientPort);
            continue;
        }

        ClientNode *client = GetOrCreateClient(&clients, clientIp, clientPort);
        if (client == NULL)
        {
            printf("Не удалось выделить память под клиента.\n");
            continue;
        }

        client->counter++;

        char answer[MAX_TEXT + 32];
        snprintf(answer, sizeof(answer), "%s %d", text, client->counter);

        printf("%s:%d -> \"%s\", ответ \"%s\"\n",
               clientIp, clientPort, text, answer);

        SendRawUdp(socketFd, serverIp, clientIp, serverPort, clientPort, answer);
    }

    FreeClients(clients);
    close(socketFd);

    printf("\nRAW echo-server завершен.\n");
    return 0;
}

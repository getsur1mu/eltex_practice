#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#define DEFAULT_PORT 5000
#define MAX_TEXT 512
#define MAX_CLIENTS 2

typedef struct Client
{
    struct sockaddr_in address;
    int active;
} Client;

volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

int SameClient(Client *client, struct sockaddr_in *address)
{
    return client->active &&
           client->address.sin_addr.s_addr == address->sin_addr.s_addr &&
           client->address.sin_port == address->sin_port;
}

int FindClient(Client *clients, struct sockaddr_in *address)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (SameClient(&clients[i], address))
            return i;
    }

    return -1;
}

int AddClient(Client *clients, struct sockaddr_in *address)
{
    int index = FindClient(clients, address);

    if (index != -1)
        return index;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!clients[i].active)
        {
            clients[i].address = *address;
            clients[i].active = 1;
            printf("Клиент %d подключен: %s:%d\n", i + 1,
                   inet_ntoa(address->sin_addr), ntohs(address->sin_port));
            return i;
        }
    }

    return -1;
}

void RemoveClient(Client *clients, int index)
{
    if (index >= 0 && index < MAX_CLIENTS)
    {
        printf("Клиент %d отключен.\n", index + 1);
        clients[index].active = 0;
    }
}

void SendToOther(int socketFd, Client *clients, int senderIndex, char *text)
{
    char message[MAX_TEXT + 64];

    snprintf(message, sizeof(message), "Клиент %d: %s", senderIndex + 1, text);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (i == senderIndex || !clients[i].active)
            continue;

        sendto(socketFd, message, strlen(message) + 1, 0,
               (struct sockaddr *)&clients[i].address, sizeof(clients[i].address));
    }
}

int main(int argc, char *argv[])
{
    int socketFd;
    int port = DEFAULT_PORT;
    struct sockaddr_in serverAddress;
    Client clients[MAX_CLIENTS];
    struct timeval timeout;

    if (argc > 1)
        port = atoi(argv[1]);

    setvbuf(stdout, NULL, _IONBF, 0);
    signal(SIGINT, SignalHandler);
    memset(clients, 0, sizeof(clients));

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1)
    {
        perror("socket");
        return 1;
    }

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("bind");
        close(socketFd);
        return 1;
    }

    printf("UDP сервер запущен на порту %d. Ctrl+C для остановки.\n", port);

    while (work)
    {
        char buffer[MAX_TEXT];
        struct sockaddr_in clientAddress;
        socklen_t addressLength = sizeof(clientAddress);
        int clientIndex;
        ssize_t bytes;

        bytes = recvfrom(socketFd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&clientAddress, &addressLength);
        if (bytes == -1)
        {
            if (!work)
                break;
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                continue;
            perror("recvfrom");
            continue;
        }

        buffer[bytes] = '\0';
        clientIndex = AddClient(clients, &clientAddress);

        if (clientIndex == -1)
        {
            char full[] = "Сервер уже занят двумя клиентами.";
            sendto(socketFd, full, sizeof(full), 0,
                   (struct sockaddr *)&clientAddress, addressLength);
            continue;
        }

        if (strcmp(buffer, "connect") == 0)
            continue;

        if (strcmp(buffer, "shutdown") == 0)
        {
            RemoveClient(clients, clientIndex);
            continue;
        }

        printf("Клиент %d: %s\n", clientIndex + 1, buffer);
        SendToOther(socketFd, clients, clientIndex, buffer);
    }

    close(socketFd);
    printf("\nUDP сервер завершен.\n");

    return 0;
}

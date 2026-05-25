#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 6000
#define MAX_CLIENTS 32
#define BUFFER_SIZE 1024
#define LINE_SIZE 512

#define MODE_LINE 0
#define MODE_FILE 1

typedef struct Client
{
    int fd;
    int active;
    int mode;
    int fileFd;
    long fileLeft;
    long fileSize;
    char fileName[300];
    char line[LINE_SIZE];
    int lineLength;
    int needClose;
} Client;

int SendAll(int socketFd, const void *buffer, size_t size)
{
    const char *data = buffer;
    size_t sent = 0;

    while (sent < size)
    {
        ssize_t result = send(socketFd, data + sent, size - sent, 0);

        if (result <= 0)
            return -1;

        sent += result;
    }

    return 0;
}

void SendText(Client *client, const char *text)
{
    SendAll(client->fd, text, strlen(text));
}

const char *BaseName(const char *path)
{
    const char *name = strrchr(path, '/');

    if (name == NULL)
        return path;

    return name + 1;
}

void InitClients(Client *clients)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;
        clients[i].fileFd = -1;
    }
}

void CloseClient(Client *client)
{
    if (client->fd != -1)
        close(client->fd);

    if (client->fileFd != -1)
        close(client->fileFd);

    memset(client, 0, sizeof(Client));
    client->fd = -1;
    client->fileFd = -1;
}

int AddClient(Client *clients, int fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!clients[i].active)
        {
            memset(&clients[i], 0, sizeof(Client));
            clients[i].fd = fd;
            clients[i].fileFd = -1;
            clients[i].active = 1;
            SendText(&clients[i], "TCP select server ready\n");
            return 0;
        }
    }

    return -1;
}

void HandleCalc(Client *client, char *line)
{
    char operation[16];
    double a;
    double b;
    double result = 0;
    char answer[LINE_SIZE];

    if (sscanf(line, "CALC %15s %lf %lf", operation, &a, &b) != 3)
    {
        SendText(client, "ERROR неверная команда CALC\n");
        return;
    }

    if (strcmp(operation, "add") == 0)
        result = a + b;
    else if (strcmp(operation, "sub") == 0)
        result = a - b;
    else if (strcmp(operation, "mul") == 0)
        result = a * b;
    else if (strcmp(operation, "div") == 0)
    {
        if (b == 0)
        {
            SendText(client, "ERROR деление на ноль\n");
            return;
        }
        result = a / b;
    }
    else
    {
        SendText(client, "ERROR неизвестная операция\n");
        return;
    }

    snprintf(answer, sizeof(answer), "RESULT %g\n", result);
    SendText(client, answer);
}

void StartFileReceive(Client *client, char *line)
{
    char name[256];
    long size;

    if (sscanf(line, "FILE %255s %ld", name, &size) != 2 || size < 0)
    {
        SendText(client, "ERROR неверная команда FILE\n");
        return;
    }

    snprintf(client->fileName, sizeof(client->fileName), "received_%s", BaseName(name));
    client->fileFd = open(client->fileName, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    if (client->fileFd == -1)
    {
        perror(client->fileName);
        SendText(client, "ERROR не удалось создать файл\n");
        return;
    }

    client->mode = MODE_FILE;
    client->fileLeft = size;
    client->fileSize = size;

    if (size == 0)
    {
        close(client->fileFd);
        client->fileFd = -1;
        client->mode = MODE_LINE;
        SendText(client, "SAVED empty file\n");
    }
}

void FinishFile(Client *client)
{
    char answer[LINE_SIZE];

    close(client->fileFd);
    client->fileFd = -1;
    client->mode = MODE_LINE;

    snprintf(answer, sizeof(answer), "SAVED %s %ld bytes\n",
             client->fileName, client->fileSize);
    SendText(client, answer);
}

void ProcessLine(Client *client)
{
    if (strncmp(client->line, "CALC ", 5) == 0)
        HandleCalc(client, client->line);
    else if (strncmp(client->line, "FILE ", 5) == 0)
        StartFileReceive(client, client->line);
    else if (strncmp(client->line, "EXIT", 4) == 0)
        client->needClose = 1;
    else
        SendText(client, "ERROR неизвестная команда\n");
}

void ProcessBytes(Client *client, char *buffer, ssize_t bytes)
{
    for (ssize_t i = 0; i < bytes && !client->needClose; i++)
    {
        if (client->mode == MODE_FILE)
        {
            if (write(client->fileFd, &buffer[i], 1) != 1)
            {
                perror("write");
                client->needClose = 1;
                return;
            }

            client->fileLeft--;
            if (client->fileLeft == 0)
                FinishFile(client);
        }
        else
        {
            if (buffer[i] == '\n')
            {
                client->line[client->lineLength] = '\0';
                ProcessLine(client);
                client->lineLength = 0;
            }
            else if (client->lineLength + 1 < LINE_SIZE)
            {
                client->line[client->lineLength] = buffer[i];
                client->lineLength++;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int serverSocket;
    int port = DEFAULT_PORT;
    struct sockaddr_in serverAddress;
    Client clients[MAX_CLIENTS];

    if (argc > 1)
        port = atoi(argv[1]);

    InitClients(clients);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("socket");
        return 1;
    }

    int option = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("bind");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1)
    {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    printf("TCP select сервер запущен на порту %d.\n", port);

    while (1)
    {
        fd_set readSet;
        int maxFd = serverSocket;

        FD_ZERO(&readSet);
        FD_SET(serverSocket, &readSet);

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].active)
            {
                FD_SET(clients[i].fd, &readSet);
                if (clients[i].fd > maxFd)
                    maxFd = clients[i].fd;
            }
        }

        if (select(maxFd + 1, &readSet, NULL, NULL, NULL) == -1)
        {
            perror("select");
            break;
        }

        if (FD_ISSET(serverSocket, &readSet))
        {
            struct sockaddr_in clientAddress;
            socklen_t addressLength = sizeof(clientAddress);
            int clientFd = accept(serverSocket, (struct sockaddr *)&clientAddress, &addressLength);

            if (clientFd == -1)
            {
                perror("accept");
            }
            else if (AddClient(clients, clientFd) != 0)
            {
                char busy[] = "ERROR сервер занят\n";
                SendAll(clientFd, busy, strlen(busy));
                close(clientFd);
            }
            else
            {
                printf("Клиент подключен: %s:%d\n",
                       inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].active && FD_ISSET(clients[i].fd, &readSet))
            {
                char buffer[BUFFER_SIZE];
                ssize_t bytes = recv(clients[i].fd, buffer, sizeof(buffer), 0);

                if (bytes <= 0)
                    clients[i].needClose = 1;
                else
                    ProcessBytes(&clients[i], buffer, bytes);

                if (clients[i].needClose)
                {
                    printf("Клиент отключен.\n");
                    CloseClient(&clients[i]);
                }
            }
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].active)
            CloseClient(&clients[i]);
    }

    close(serverSocket);
    return 0;
}

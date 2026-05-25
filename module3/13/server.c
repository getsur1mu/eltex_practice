#include "common.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *BaseName(const char *path)
{
    const char *name = strrchr(path, '/');

    if (name == NULL)
        return path;

    return name + 1;
}

void SendText(int client, const char *text)
{
    SendAll(client, text, strlen(text));
}

void HandleCalc(int client, char *line)
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

void HandleFile(int client, char *line)
{
    char name[256];
    char savedName[300];
    long size;
    long received = 0;
    int fd;
    char buffer[BUFFER_SIZE];
    char answer[LINE_SIZE];

    if (sscanf(line, "FILE %255s %ld", name, &size) != 2 || size < 0)
    {
        SendText(client, "ERROR неверная команда FILE\n");
        return;
    }

    snprintf(savedName, sizeof(savedName), "received_%s", BaseName(name));

    fd = open(savedName, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd == -1)
    {
        perror(savedName);
        SendText(client, "ERROR не удалось создать файл\n");
        return;
    }

    while (received < size)
    {
        ssize_t need = size - received;
        ssize_t bytes;

        if (need > BUFFER_SIZE)
            need = BUFFER_SIZE;

        bytes = recv(client, buffer, need, 0);
        if (bytes <= 0)
        {
            close(fd);
            return;
        }

        if (write(fd, buffer, bytes) != bytes)
        {
            perror("write");
            close(fd);
            SendText(client, "ERROR ошибка записи файла\n");
            return;
        }

        received += bytes;
    }

    close(fd);
    snprintf(answer, sizeof(answer), "SAVED %s %ld bytes\n", savedName, size);
    SendText(client, answer);
}

void HandleClient(int client)
{
    char line[LINE_SIZE];

    SendText(client, "TCP server ready\n");

    while (1)
    {
        ssize_t bytes = ReceiveLine(client, line, sizeof(line));

        if (bytes <= 0)
            break;

        if (strncmp(line, "CALC ", 5) == 0)
            HandleCalc(client, line);
        else if (strncmp(line, "FILE ", 5) == 0)
            HandleFile(client, line);
        else if (strncmp(line, "EXIT", 4) == 0)
            break;
        else
            SendText(client, "ERROR неизвестная команда\n");
    }
}

int main(int argc, char *argv[])
{
    int serverSocket;
    int port = DEFAULT_PORT;
    struct sockaddr_in serverAddress;

    if (argc > 1)
        port = atoi(argv[1]);

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

    printf("TCP сервер запущен на порту %d.\n", port);

    while (1)
    {
        struct sockaddr_in clientAddress;
        socklen_t addressLength = sizeof(clientAddress);
        int client = accept(serverSocket, (struct sockaddr *)&clientAddress, &addressLength);

        if (client == -1)
        {
            perror("accept");
            continue;
        }

        printf("Клиент подключен: %s:%d\n",
               inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

        HandleClient(client);
        close(client);
        printf("Клиент отключен.\n");
    }

    close(serverSocket);
    return 0;
}

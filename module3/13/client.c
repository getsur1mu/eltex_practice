#include "common.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

const char *BaseName(const char *path)
{
    const char *name = strrchr(path, '/');

    if (name == NULL)
        return path;

    return name + 1;
}

int ConnectToServer(const char *ip, int port)
{
    int socketFd;
    struct sockaddr_in serverAddress;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        perror("socket");
        return -1;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serverAddress.sin_addr) != 1)
    {
        printf("Неверный адрес сервера.\n");
        close(socketFd);
        return -1;
    }

    if (connect(socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("connect");
        close(socketFd);
        return -1;
    }

    return socketFd;
}

void PrintAnswer(int socketFd)
{
    char answer[LINE_SIZE];

    if (ReceiveLine(socketFd, answer, sizeof(answer)) > 0)
        printf("%s", answer);
}

void SendCalcCommand(int socketFd, char *line)
{
    char operation[16];
    double a;
    double b;
    char command[LINE_SIZE];

    if (sscanf(line, "%15s %lf %lf", operation, &a, &b) != 3)
    {
        printf("Формат: add/sub/mul/div <число> <число>\n");
        return;
    }

    snprintf(command, sizeof(command), "CALC %s %g %g\n", operation, a, b);
    SendAll(socketFd, command, strlen(command));
    PrintAnswer(socketFd);
}

void SendFileCommand(int socketFd, char *line)
{
    char path[256];
    struct stat st;
    int fd;
    char command[LINE_SIZE];
    char buffer[BUFFER_SIZE];
    long sent = 0;

    if (sscanf(line, "send %255s", path) != 1)
    {
        printf("Формат: send <имя файла>\n");
        return;
    }

    if (stat(path, &st) == -1)
    {
        perror(path);
        return;
    }

    fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        perror(path);
        return;
    }

    snprintf(command, sizeof(command), "FILE %s %ld\n", BaseName(path), (long)st.st_size);
    if (SendAll(socketFd, command, strlen(command)) != 0)
    {
        close(fd);
        return;
    }

    while (sent < st.st_size)
    {
        ssize_t bytes = read(fd, buffer, sizeof(buffer));

        if (bytes <= 0)
            break;

        if (SendAll(socketFd, buffer, bytes) != 0)
            break;

        sent += bytes;
    }

    close(fd);
    PrintAnswer(socketFd);
}

int main(int argc, char *argv[])
{
    char ip[64] = "127.0.0.1";
    int port = DEFAULT_PORT;
    int socketFd;
    char line[LINE_SIZE];

    if (argc > 1)
        strncpy(ip, argv[1], sizeof(ip) - 1);
    if (argc > 2)
        port = atoi(argv[2]);

    socketFd = ConnectToServer(ip, port);
    if (socketFd == -1)
        return 1;

    PrintAnswer(socketFd);

    printf("Команды: add/sub/mul/div a b, send file, exit\n");

    while (1)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';

        if (strncmp(line, "send ", 5) == 0)
            SendFileCommand(socketFd, line);
        else if (strncmp(line, "add ", 4) == 0 ||
                 strncmp(line, "sub ", 4) == 0 ||
                 strncmp(line, "mul ", 4) == 0 ||
                 strncmp(line, "div ", 4) == 0)
            SendCalcCommand(socketFd, line);
        else if (strcmp(line, "exit") == 0)
        {
            SendAll(socketFd, "EXIT\n", 5);
            break;
        }
        else if (strlen(line) != 0)
            printf("Неизвестная команда.\n");
    }

    close(socketFd);
    return 0;
}

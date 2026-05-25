#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 5000
#define MAX_TEXT 512

int main(int argc, char *argv[])
{
    int socketFd;
    int port = DEFAULT_PORT;
    struct sockaddr_in serverAddress;
    char serverIp[64] = "127.0.0.1";
    char buffer[MAX_TEXT];

    if (argc > 1)
        strncpy(serverIp, argv[1], sizeof(serverIp) - 1);
    if (argc > 2)
        port = atoi(argv[2]);

    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1)
    {
        perror("socket");
        return 1;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIp, &serverAddress.sin_addr) != 1)
    {
        printf("Неверный адрес сервера.\n");
        close(socketFd);
        return 1;
    }

    sendto(socketFd, "connect", 8, 0,
           (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    printf("UDP клиент. Для выхода введите shutdown.\n");

    while (1)
    {
        fd_set readSet;
        int maxFd;

        FD_ZERO(&readSet);
        FD_SET(STDIN_FILENO, &readSet);
        FD_SET(socketFd, &readSet);

        maxFd = socketFd > STDIN_FILENO ? socketFd : STDIN_FILENO;

        printf("> ");
        fflush(stdout);

        if (select(maxFd + 1, &readSet, NULL, NULL, NULL) == -1)
        {
            perror("select");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readSet))
        {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL)
                break;

            buffer[strcspn(buffer, "\n")] = '\0';
            if (strlen(buffer) == 0)
                continue;

            sendto(socketFd, buffer, strlen(buffer) + 1, 0,
                   (struct sockaddr *)&serverAddress, sizeof(serverAddress));

            if (strcmp(buffer, "shutdown") == 0)
                break;
        }

        if (FD_ISSET(socketFd, &readSet))
        {
            ssize_t bytes = recvfrom(socketFd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);

            if (bytes > 0)
            {
                buffer[bytes] = '\0';
                printf("\n%s\n", buffer);
            }
        }
    }

    close(socketFd);
    return 0;
}

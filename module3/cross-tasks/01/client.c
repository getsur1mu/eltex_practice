#include "common.h"

#include <errno.h>
#include <signal.h>
#include <sys/time.h>

volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

void SendClose(int socketFd, const char *clientIp, const char *serverIp,
               unsigned short clientPort, unsigned short serverPort)
{
    SendRawUdp(socketFd, clientIp, serverIp, clientPort, serverPort, CLOSE_MESSAGE);
}

void WaitAnswer(int socketFd, unsigned short clientPort, unsigned short serverPort)
{
    char sourceIp[INET_ADDRSTRLEN];
    unsigned short sourcePort = 0;
    char text[MAX_TEXT];

    while (work)
    {
        int result = ReadRawUdp(socketFd, clientPort, serverPort,
                                sourceIp, sizeof(sourceIp),
                                &sourcePort, text, sizeof(text));

        if (result == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                printf("Ответ не получен.\n");
                return;
            }
            if (errno == EINTR)
                continue;

            perror("recv");
            return;
        }

        if (result == 1)
        {
            printf("Ответ от сервера: %s\n", text);
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    const char *serverIp = DEFAULT_SERVER_IP;
    const char *clientIp = DEFAULT_SERVER_IP;
    unsigned short serverPort = DEFAULT_SERVER_PORT;
    unsigned short clientPort = DEFAULT_CLIENT_PORT;
    int socketFd;
    char line[MAX_TEXT];
    struct timeval timeout;

    if (argc > 1)
        serverIp = argv[1];
    if (argc > 2)
        serverPort = (unsigned short)atoi(argv[2]);
    if (argc > 3)
        clientPort = (unsigned short)atoi(argv[3]);
    if (argc > 4)
        clientIp = argv[4];

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    srand(time(NULL) ^ getpid());

    socketFd = CreateRawSocket();
    if (socketFd == -1)
        return 1;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    setsockopt(socketFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    printf("RAW echo-client %s:%d -> %s:%d\n",
           clientIp, clientPort, serverIp, serverPort);
    printf("Введите сообщение, exit или close для выхода.\n");

    while (work)
    {
        printf("> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0)
            continue;

        if (strcmp(line, "exit") == 0 || strcmp(line, CLOSE_MESSAGE) == 0)
            break;

        if (SendRawUdp(socketFd, clientIp, serverIp, clientPort, serverPort, line) == 0)
            WaitAnswer(socketFd, clientPort, serverPort);
    }

    SendClose(socketFd, clientIp, serverIp, clientPort, serverPort);
    close(socketFd);

    printf("\nКлиент завершен, серверу отправлено сообщение close.\n");
    return 0;
}

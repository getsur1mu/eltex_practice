#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 6000
#define BUFFER_SIZE 1024
#define LINE_SIZE 512

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

ssize_t ReceiveLine(int socketFd, char *line, size_t size)
{
    size_t count = 0;

    while (count + 1 < size)
    {
        char ch;
        ssize_t result = recv(socketFd, &ch, 1, 0);

        if (result <= 0)
            return result;

        line[count] = ch;
        count++;

        if (ch == '\n')
            break;
    }

    line[count] = '\0';
    return count;
}

#endif

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_PORT 5000
#define BUFFER_SIZE 65536

volatile sig_atomic_t work = 1;

void SignalHandler(int signalNumber)
{
    (void)signalNumber;
    work = 0;
}

void PrintPayload(FILE *out, const unsigned char *payload, int length)
{
    fprintf(out, "ASCII: ");
    for (int i = 0; i < length; i++)
    {
        if (payload[i] >= 32 && payload[i] <= 126)
            fputc(payload[i], out);
        else
            fputc('.', out);
    }

    fprintf(out, "\nHEX: ");
    for (int i = 0; i < length; i++)
    {
        fprintf(out, "%02x ", payload[i]);
    }

    fprintf(out, "\n\n");
    fflush(out);
}

void ProcessPacket(FILE *out, unsigned char *buffer, ssize_t bytes, int filterPort)
{
    struct iphdr *ipHeader = (struct iphdr *)buffer;
    struct udphdr *udpHeader;
    unsigned char *payload;
    int ipHeaderLength;
    int payloadLength;
    char sourceIp[INET_ADDRSTRLEN];
    char destinationIp[INET_ADDRSTRLEN];
    struct in_addr sourceAddress;
    struct in_addr destinationAddress;

    if (bytes < (ssize_t)(sizeof(struct iphdr) + sizeof(struct udphdr)))
        return;

    ipHeaderLength = ipHeader->ihl * 4;
    if (bytes < ipHeaderLength + (ssize_t)sizeof(struct udphdr))
        return;

    udpHeader = (struct udphdr *)(buffer + ipHeaderLength);
    if (ntohs(udpHeader->dest) != filterPort)
        return;

    payload = buffer + ipHeaderLength + sizeof(struct udphdr);
    payloadLength = ntohs(udpHeader->len) - sizeof(struct udphdr);

    if (payloadLength <= 0)
        return;

    sourceAddress.s_addr = ipHeader->saddr;
    destinationAddress.s_addr = ipHeader->daddr;
    inet_ntop(AF_INET, &sourceAddress, sourceIp, sizeof(sourceIp));
    inet_ntop(AF_INET, &destinationAddress, destinationIp, sizeof(destinationIp));

    fprintf(out, "UDP packet %s:%d -> %s:%d, payload %d bytes\n",
            sourceIp, ntohs(udpHeader->source),
            destinationIp, ntohs(udpHeader->dest),
            payloadLength);

    PrintPayload(out, payload, payloadLength);
}

int main(int argc, char *argv[])
{
    int rawSocket;
    int port = DEFAULT_PORT;
    const char *dumpFile = "udp_dump.txt";
    FILE *out;
    unsigned char buffer[BUFFER_SIZE];

    if (argc > 1)
        port = atoi(argv[1]);
    if (argc > 2)
        dumpFile = argv[2];

    signal(SIGINT, SignalHandler);

    out = fopen(dumpFile, "a");
    if (out == NULL)
    {
        perror(dumpFile);
        return 1;
    }

    rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (rawSocket == -1)
    {
        perror("socket");
        printf("RAW-сокеты обычно требуют запуск через sudo.\n");
        fclose(out);
        return 1;
    }

    printf("Снифер запущен. Фильтр UDP dst port %d. Дамп: %s\n", port, dumpFile);
    printf("Для остановки нажмите Ctrl+C.\n");

    while (work)
    {
        ssize_t bytes = recv(rawSocket, buffer, sizeof(buffer), 0);

        if (bytes == -1)
        {
            if (!work)
                break;
            perror("recv");
            continue;
        }

        ProcessPacket(out, buffer, bytes, port);
    }

    close(rawSocket);
    fclose(out);

    return 0;
}

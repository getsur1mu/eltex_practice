#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 5000
#define DEFAULT_CLIENT_PORT 40000
#define MAX_TEXT 512
#define CLOSE_MESSAGE "close"

unsigned short Checksum(unsigned short *buffer, int size)
{
    unsigned long sum = 0;

    while (size > 1)
    {
        sum += *buffer;
        buffer++;
        size -= 2;
    }

    if (size == 1)
        sum += *(unsigned char *)buffer;

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);

    return (unsigned short)(~sum);
}

int CreateRawSocket()
{
    int socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    int option = 1;

    if (socketFd == -1)
    {
        perror("socket");
        printf("RAW-сокеты обычно требуют запуск через sudo.\n");
        return -1;
    }

    if (setsockopt(socketFd, IPPROTO_IP, IP_HDRINCL, &option, sizeof(option)) == -1)
    {
        perror("setsockopt IP_HDRINCL");
        close(socketFd);
        return -1;
    }

    return socketFd;
}

int SendRawUdp(int socketFd,
               const char *sourceIp,
               const char *destinationIp,
               unsigned short sourcePort,
               unsigned short destinationPort,
               const char *text)
{
    unsigned char packet[sizeof(struct iphdr) + sizeof(struct udphdr) + MAX_TEXT];
    struct iphdr *ipHeader = (struct iphdr *)packet;
    struct udphdr *udpHeader = (struct udphdr *)(packet + sizeof(struct iphdr));
    char *payload = (char *)(packet + sizeof(struct iphdr) + sizeof(struct udphdr));
    struct sockaddr_in destinationAddress;
    int payloadLength = strlen(text);
    int packetLength = sizeof(struct iphdr) + sizeof(struct udphdr) + payloadLength;

    if (payloadLength >= MAX_TEXT)
        payloadLength = MAX_TEXT - 1;

    memset(packet, 0, sizeof(packet));
    memcpy(payload, text, payloadLength);
    packetLength = sizeof(struct iphdr) + sizeof(struct udphdr) + payloadLength;

    ipHeader->ihl = 5;
    ipHeader->version = 4;
    ipHeader->tos = 0;
    ipHeader->tot_len = htons(packetLength);
    ipHeader->id = htons(rand() % 65535);
    ipHeader->frag_off = 0;
    ipHeader->ttl = 64;
    ipHeader->protocol = IPPROTO_UDP;
    ipHeader->saddr = inet_addr(sourceIp);
    ipHeader->daddr = inet_addr(destinationIp);
    ipHeader->check = 0;
    ipHeader->check = Checksum((unsigned short *)ipHeader, sizeof(struct iphdr));

    udpHeader->source = htons(sourcePort);
    udpHeader->dest = htons(destinationPort);
    udpHeader->len = htons(sizeof(struct udphdr) + payloadLength);
    udpHeader->check = 0;

    memset(&destinationAddress, 0, sizeof(destinationAddress));
    destinationAddress.sin_family = AF_INET;
    destinationAddress.sin_addr.s_addr = ipHeader->daddr;
    destinationAddress.sin_port = htons(destinationPort);

    if (sendto(socketFd, packet, packetLength, 0,
               (struct sockaddr *)&destinationAddress, sizeof(destinationAddress)) == -1)
    {
        perror("sendto");
        return -1;
    }

    return 0;
}

int ReadRawUdp(int socketFd,
               unsigned short filterDestinationPort,
               unsigned short filterSourcePort,
               char *sourceIp,
               size_t sourceIpSize,
               unsigned short *sourcePort,
               char *text,
               size_t textSize)
{
    unsigned char buffer[65536];
    struct iphdr *ipHeader;
    struct udphdr *udpHeader;
    char *payload;
    int ipHeaderLength;
    int payloadLength;
    struct in_addr sourceAddress;
    ssize_t bytes = recv(socketFd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
        return -1;

    ipHeader = (struct iphdr *)buffer;
    if (ipHeader->protocol != IPPROTO_UDP)
        return 0;

    ipHeaderLength = ipHeader->ihl * 4;
    if (bytes < ipHeaderLength + (ssize_t)sizeof(struct udphdr))
        return 0;

    udpHeader = (struct udphdr *)(buffer + ipHeaderLength);

    if (filterDestinationPort != 0 && ntohs(udpHeader->dest) != filterDestinationPort)
        return 0;
    if (filterSourcePort != 0 && ntohs(udpHeader->source) != filterSourcePort)
        return 0;

    payloadLength = ntohs(udpHeader->len) - sizeof(struct udphdr);
    if (payloadLength <= 0)
        return 0;
    if ((size_t)payloadLength >= textSize)
        payloadLength = textSize - 1;

    payload = (char *)(buffer + ipHeaderLength + sizeof(struct udphdr));
    memcpy(text, payload, payloadLength);
    text[payloadLength] = '\0';

    sourceAddress.s_addr = ipHeader->saddr;
    inet_ntop(AF_INET, &sourceAddress, sourceIp, sourceIpSize);
    *sourcePort = ntohs(udpHeader->source);

    return 1;
}

#endif

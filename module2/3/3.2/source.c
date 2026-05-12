#include "source.h"

void StartNetwork(int argc, char *argv[])
{
    uint32_t gateway;
    uint32_t mask;
    uint32_t packetIp;
    uint32_t network;
    int packets;
    int localPackets = 0;
    int otherPackets = 0;

    if (argc != 4)
    {
        printf("Использование:\n");
        printf("./subnet_check <ip шлюза> <маска подсети> <количество пакетов>\n");
        printf("Пример:\n");
        printf("./subnet_check 192.168.1.1 255.255.255.0 10\n");
        return;
    }

    if (ParseIp(argv[1], &gateway) != 0)
    {
        printf("Неверный ip шлюза.\n");
        return;
    }

    if (ParseIp(argv[2], &mask) != 0)
    {
        printf("Неверная маска подсети.\n");
        return;
    }

    packets = atoi(argv[3]);
    if (packets <= 0)
    {
        printf("Количество пакетов должно быть больше нуля.\n");
        return;
    }

    srand(time(NULL));

    network = gateway & mask;

    printf("== ПРОВЕРКА ПАКЕТОВ IPv4 ==\n\n");
    printf("Шлюз: ");
    PrintIp(gateway);
    printf("\nМаска: ");
    PrintIp(mask);
    printf("\nПодсеть: ");
    PrintIp(network);
    printf("\n\n");

    for (int i = 0; i < packets; i++)
    {
        packetIp = GenerateIp();

        printf("Пакет %d: ", i + 1);
        PrintIp(packetIp);

        if (IsLocalNetwork(gateway, mask, packetIp))
        {
            printf(" -> своя подсеть\n");
            localPackets++;
        }
        else
        {
            printf(" -> другая сеть\n");
            otherPackets++;
        }
    }

    printf("\n== СТАТИСТИКА ==\n");
    printf("Всего пакетов: %d\n", packets);
    printf("Своя подсеть: %d (%.2f%%)\n", localPackets, (double)localPackets * 100 / packets);
    printf("Другие сети: %d (%.2f%%)\n", otherPackets, (double)otherPackets * 100 / packets);
}

int ParseIp(char *str, uint32_t *ip)
{
    struct in_addr temp;

    if (inet_pton(AF_INET, str, &temp) != 1)
    {
        return -1;
    }

    *ip = ntohl(temp.s_addr);
    return 0;
}

uint32_t GenerateIp()
{
    uint32_t ip = 0;

    ip |= (rand() % 256) << 24;
    ip |= (rand() % 256) << 16;
    ip |= (rand() % 256) << 8;
    ip |= rand() % 256;

    return ip;
}

void PrintIp(uint32_t ip)
{
    printf("%u.%u.%u.%u",
           (ip >> 24) & 255,
           (ip >> 16) & 255,
           (ip >> 8) & 255,
           ip & 255);
}

int IsLocalNetwork(uint32_t gateway, uint32_t mask, uint32_t ip)
{
    if ((gateway & mask) == (ip & mask))
        return 1;

    return 0;
}

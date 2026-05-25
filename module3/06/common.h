#ifndef COMMON_H
#define COMMON_H

#define QUEUE_KEY 0x060610
#define SERVER_TYPE 10
#define MAX_TEXT 256
#define MAX_CLIENTS 32

typedef struct Message
{
    long mtype;
    int sender;
    char text[MAX_TEXT];
} Message;

#endif

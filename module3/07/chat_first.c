#include "common.h"

#include <mqueue.h>
#include <stdio.h>
#include <string.h>

int OpenQueues(mqd_t *sendQueue, mqd_t *receiveQueue)
{
    struct mq_attr attr;

    mq_unlink(QUEUE_TO_FIRST);
    mq_unlink(QUEUE_TO_SECOND);

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_TEXT;
    attr.mq_curmsgs = 0;

    *sendQueue = mq_open(QUEUE_TO_SECOND, O_CREAT | O_WRONLY, 0666, &attr);
    if (*sendQueue == (mqd_t)-1)
    {
        perror("mq_open send");
        return -1;
    }

    *receiveQueue = mq_open(QUEUE_TO_FIRST, O_CREAT | O_RDONLY, 0666, &attr);
    if (*receiveQueue == (mqd_t)-1)
    {
        perror("mq_open receive");
        mq_close(*sendQueue);
        return -1;
    }

    return 0;
}

int SendText(mqd_t queue)
{
    char text[MAX_TEXT];
    unsigned int priority = NORMAL_PRIORITY;

    printf("Вы: ");
    fflush(stdout);

    if (fgets(text, sizeof(text), stdin) == NULL)
        return 0;

    text[strcspn(text, "\n")] = '\0';
    if (strcmp(text, "shutdown") == 0)
        priority = FINISH_PRIORITY;

    if (mq_send(queue, text, strlen(text) + 1, priority) == -1)
    {
        perror("mq_send");
        return 0;
    }

    return priority != FINISH_PRIORITY;
}

int ReceiveText(mqd_t queue)
{
    char text[MAX_TEXT];
    unsigned int priority;

    if (mq_receive(queue, text, sizeof(text), &priority) == -1)
    {
        perror("mq_receive");
        return 0;
    }

    if (priority == FINISH_PRIORITY)
    {
        printf("Собеседник завершил чат.\n");
        return 0;
    }

    printf("Собеседник: %s\n", text);
    return 1;
}

int main()
{
    mqd_t sendQueue;
    mqd_t receiveQueue;
    int work = 1;

    if (OpenQueues(&sendQueue, &receiveQueue) != 0)
        return 1;

    printf("Первый участник чата. Для выхода введите shutdown.\n");

    while (work)
    {
        work = SendText(sendQueue);
        if (work)
            work = ReceiveText(receiveQueue);
    }

    mq_close(sendQueue);
    mq_close(receiveQueue);
    mq_unlink(QUEUE_TO_FIRST);
    mq_unlink(QUEUE_TO_SECOND);

    return 0;
}

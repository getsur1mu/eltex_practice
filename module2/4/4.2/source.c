#include "source.h"

void StartQueue()
{
    QueueNode *head = NULL;
    QueueNode *tail = NULL;
    Message message;
    int choice = -1;
    int lastId = 0;
    int count;
    int priority;

    srand(time(NULL));
    GenerateMessages(&head, &tail, 10, &lastId);

    printf("= ОЧЕРЕДЬ С ПРИОРИТЕТОМ =\n");

    while (choice != 0)
    {
        printf("\nВведите необходимую операцию:\n");
        printf("1.Вывести очередь\n2.Добавить случайные сообщения\n3.Извлечь первый элемент\n");
        printf("4.Извлечь с указанным приоритетом\n5.Извлечь с приоритетом не ниже заданного\n0.Выход\n-->");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                ShowQueue(head);
                break;
            case 2:
                printf("Сколько сообщений добавить?\n->");
                scanf("%d", &count);
                GenerateMessages(&head, &tail, count, &lastId);
                break;
            case 3:
                if (PopFirst(&head, &tail, &message))
                    ShowMessage(message);
                else
                    printf("Очередь пуста.\n");
                break;
            case 4:
                printf("Введите приоритет 0-255:\n->");
                scanf("%d", &priority);
                if (PopByPriority(&head, &tail, priority, &message))
                    ShowMessage(message);
                else
                    printf("Сообщение с таким приоритетом не найдено.\n");
                break;
            case 5:
                printf("Введите минимальный приоритет 0-255:\n->");
                scanf("%d", &priority);
                if (PopNotLowerPriority(&head, &tail, priority, &message))
                    ShowMessage(message);
                else
                    printf("Подходящее сообщение не найдено.\n");
                break;
            case 0:
                printf("Хорошего дня :)\n");
                break;
            default:
                printf("Введена неверная команда.\n");
                break;
        }
    }

    FreeQueue(head);
}

void AddMessage(QueueNode **head, QueueNode **tail, Message message)
{
    QueueNode *node = malloc(sizeof(QueueNode));

    if (node == NULL)
    {
        printf("Не удалось выделить память.\n");
        return;
    }

    node->message = message;
    node->next = NULL;

    if (*head == NULL)
    {
        *head = node;
        *tail = node;
    }
    else
    {
        (*tail)->next = node;
        *tail = node;
    }
}

int PopFirst(QueueNode **head, QueueNode **tail, Message *message)
{
    QueueNode *temp;

    if (*head == NULL)
        return 0;

    temp = *head;
    *message = temp->message;
    *head = (*head)->next;

    if (*head == NULL)
        *tail = NULL;

    free(temp);
    return 1;
}

int PopByPriority(QueueNode **head, QueueNode **tail, int priority, Message *message)
{
    QueueNode *current = *head;
    QueueNode *prev = NULL;

    while (current != NULL)
    {
        if (current->message.priority == priority)
        {
            *message = current->message;

            if (prev == NULL)
                *head = current->next;
            else
                prev->next = current->next;

            if (current == *tail)
                *tail = prev;

            free(current);
            return 1;
        }

        prev = current;
        current = current->next;
    }

    return 0;
}

int PopNotLowerPriority(QueueNode **head, QueueNode **tail, int priority, Message *message)
{
    QueueNode *current = *head;
    QueueNode *prev = NULL;

    while (current != NULL)
    {
        if (current->message.priority >= priority)
        {
            *message = current->message;

            if (prev == NULL)
                *head = current->next;
            else
                prev->next = current->next;

            if (current == *tail)
                *tail = prev;

            free(current);
            return 1;
        }

        prev = current;
        current = current->next;
    }

    return 0;
}

void GenerateMessages(QueueNode **head, QueueNode **tail, int count, int *lastId)
{
    Message message;

    if (count < 0)
        count = 0;

    for (int i = 0; i < count; i++)
    {
        (*lastId)++;
        message.id = *lastId;
        message.priority = rand() % (MAX_PRIORITY + 1);
        sprintf(message.text, "Message number %d", message.id);
        AddMessage(head, tail, message);
    }
}

void ShowQueue(QueueNode *head)
{
    QueueNode *current = head;
    int i = 1;

    if (head == NULL)
    {
        printf("Очередь пуста.\n");
        return;
    }

    while (current != NULL)
    {
        printf("%d. id=%d priority=%d text=\"%s\"\n",
               i,
               current->message.id,
               current->message.priority,
               current->message.text);
        current = current->next;
        i++;
    }
}

void ShowMessage(Message message)
{
    printf("Извлечено сообщение:\n");
    printf("id=%d priority=%d text=\"%s\"\n", message.id, message.priority, message.text);
}

void FreeQueue(QueueNode *head)
{
    QueueNode *current = head;
    QueueNode *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

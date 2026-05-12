#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TEXT 100
#define MAX_PRIORITY 255

typedef struct Message{
    int id;
    int priority;
    char text[MAX_TEXT];
}Message;

typedef struct QueueNode{
    Message message;
    struct QueueNode *next;
}QueueNode;

void StartQueue();
void AddMessage(QueueNode **head, QueueNode **tail, Message message);
int PopFirst(QueueNode **head, QueueNode **tail, Message *message);
int PopByPriority(QueueNode **head, QueueNode **tail, int priority, Message *message);
int PopNotLowerPriority(QueueNode **head, QueueNode **tail, int priority, Message *message);
void GenerateMessages(QueueNode **head, QueueNode **tail, int count, int *lastId);
void ShowQueue(QueueNode *head);
void ShowMessage(Message message);
void FreeQueue(QueueNode *head);

#endif

#include "list.h"
#include <stdlib.h>
#include <string.h>

BookNode *CreateNode(Person person)
{
    BookNode *node = malloc(sizeof(BookNode));

    if (node == NULL)
        return NULL;

    node->person = person;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

void AddPersonToList(BookNode **head, Person person)
{
    BookNode *newNode = CreateNode(person);
    BookNode *current = *head;

    if (newNode == NULL)
        return;

    if (*head == NULL)
    {
        *head = newNode;
        return;
    }

    while (current != NULL && ComparePersons(current->person, person) < 0)
        current = current->next;

    if (current == *head)
    {
        newNode->next = *head;
        (*head)->prev = newNode;
        *head = newNode;
    }
    else if (current == NULL)
    {
        current = *head;
        while (current->next != NULL)
            current = current->next;
        current->next = newNode;
        newNode->prev = current;
    }
    else
    {
        newNode->next = current;
        newNode->prev = current->prev;
        current->prev->next = newNode;
        current->prev = newNode;
    }
}

void RemovePersonFromList(BookNode **head, int number)
{
    BookNode *node = GetNodeByIndex(*head, number);

    if (node == NULL)
        return;

    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        *head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;

    free(node);
}

BookNode *GetNodeByIndex(BookNode *head, int index)
{
    int i = 1;
    BookNode *current = head;

    while (current != NULL)
    {
        if (i == index)
            return current;

        current = current->next;
        i++;
    }

    return NULL;
}

int ComparePersons(Person a, Person b)
{
    int result = strcmp(a.lastName, b.lastName);

    if (result == 0)
        result = strcmp(a.firstName, b.firstName);

    return result;
}

int CountPersons(BookNode *head)
{
    int count = 0;

    while (head != NULL)
    {
        count++;
        head = head->next;
    }

    return count;
}

void FreeBook(BookNode *head)
{
    BookNode *next;

    while (head != NULL)
    {
        next = head->next;
        free(head);
        head = next;
    }
}

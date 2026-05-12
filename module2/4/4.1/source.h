#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100
#define MAX_AMOUNT 5

typedef struct Person{
    char firstName[MAX_LENGTH];
    char lastName[MAX_LENGTH];
    char patrynomic[MAX_LENGTH];
    char placeOfWork[MAX_LENGTH];
    char post[MAX_LENGTH];
    char phoneNumber[MAX_AMOUNT][MAX_LENGTH];
    char email[MAX_AMOUNT][MAX_LENGTH];
    char link[MAX_AMOUNT][MAX_LENGTH];
    int fields[8];
}Person;

typedef struct BookNode{
    Person person;
    struct BookNode *prev;
    struct BookNode *next;
}BookNode;

void Menu();
void ShowPersons(BookNode *head);
void AddPerson(BookNode **head);
void AddPersonToList(BookNode **head, Person person);
void ChangePersonInformation(BookNode **head, int personToChange);
void RemovePerson(BookNode **head, int personToRemove);
void FreeBook(BookNode *head);
void TestPersons(BookNode **head);

BookNode *CreateNode(Person person);
BookNode *GetNodeByIndex(BookNode *head, int index);
int InputOptionalString(char *buffer, int size, const char *prompt);
int ComparePersons(Person a, Person b);
int CountPersons(BookNode *head);

#endif

#ifndef LIST_H
#define LIST_H

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

BookNode *CreateNode(Person person);
void AddPersonToList(BookNode **head, Person person);
void RemovePersonFromList(BookNode **head, int number);
BookNode *GetNodeByIndex(BookNode *head, int index);
int ComparePersons(Person a, Person b);
int CountPersons(BookNode *head);
void FreeBook(BookNode *head);

#endif

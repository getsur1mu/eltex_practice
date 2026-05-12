#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 100
#define MAX_AMOUNT 5
#define BALANCE_PERIOD 3

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

typedef struct TreeNode{
    Person person;
    struct TreeNode *left;
    struct TreeNode *right;
}TreeNode;

void Menu();
void AddPerson(TreeNode **root, int *changes);
void AddPersonToTree(TreeNode **root, Person person);
void ShowPersons(TreeNode *root, int *number);
void ChangePersonInformation(TreeNode **root, int personToChange, int *changes);
void RemovePerson(TreeNode **root, int personToRemove, int *changes);
void TestPersons(TreeNode **root);
void FreeTree(TreeNode *root);
void CheckBalance(TreeNode **root, int *changes);

TreeNode *CreateNode(Person person);
TreeNode *GetNodeByIndex(TreeNode *root, int index, int *currentIndex);
TreeNode *RemoveByPerson(TreeNode *root, Person person);
TreeNode *FindMin(TreeNode *root);
TreeNode *BuildBalanced(Person *array, int left, int right);

int CountPersons(TreeNode *root);
int ComparePersons(Person a, Person b);
int InputOptionalString(char *buffer, int size, const char *prompt);
void FillArray(TreeNode *root, Person *array, int *index);

#endif

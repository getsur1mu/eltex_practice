#ifndef SOURCE_H
#define SOURCE_H

#include "list.h"
#include <stdio.h>
#include <string.h>

void Menu();
void AddPerson(BookNode **head);
void ShowPersons(BookNode *head);
void ChangePersonInformation(BookNode **head, int number);
void TestPersons(BookNode **head);
int InputOptionalString(char *buffer, int size, const char *prompt);

#endif

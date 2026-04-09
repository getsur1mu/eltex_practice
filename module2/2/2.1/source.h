#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>

#define MAX_LENGTH 20
#define MAX_AMOUNT 5

typedef struct Person{
    char firstName[MAX_LENGTH]; //Имя
    char lastName[MAX_LENGTH];  //Фамилия
    char patrynomic[MAX_LENGTH];//Отчество 
    char placeOfWork[MAX_LENGTH];//Место работы
    char post[MAX_LENGTH];//Должность
    char email[MAX_AMOUNT][MAX_LENGTH];//Почты
    char link[MAX_AMOUNT][MAX_LENGTH];//Ссылки
    int amountOfEmails;
    int amountOfLinks;
}Person;

void ShowPersons(Person* book, int currentSize);
int AddPerson(Person* book, int maxSize, int currentSize);
int ChangePersonInformation(Person* book, int maxSize, int currentSize);
int RemovePerson(Person* book, int maxSize, int currentSize);
int SizeOfBook(Person* book, int maxSize, int currentSize);

int InputOptionalString(char *buffer, int size, const char *prompt);

void Menu(Person* book, int size);

#endif
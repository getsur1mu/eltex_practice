#include "source.h"
#include <string.h>



int AddPerson(Person* book, int maxSize, int currentSize)
{   
    if (currentSize>=maxSize)
    {
        printf("Книга переполнена, невозможно добавить новую запись.\n");
        return currentSize;
    }
    book[currentSize].amountOfEmails = 0;
    book[currentSize].amountOfLinks = 0;

    printf("Добавление записи:\n*-обязательное поле\n");
    printf("Введите имя*:\n->");
    scanf("%s", book[currentSize].firstName);
    printf("Введите фамилию*:\n->");
    scanf("%s", book[currentSize].lastName);

    if(InputOptionalString(book[currentSize].patrynomic, MAX_LENGTH, "Введите отчество")) {
        printf("Отчество: %s\n", book[currentSize].patrynomic);
    }
    if(InputOptionalString(book[currentSize].patrynomic, MAX_LENGTH, "Введите место работы")) {
        printf("Место работы: %s\n", book[currentSize].placeOfWork);
    }
    if(InputOptionalString(book[currentSize].patrynomic, MAX_LENGTH, "Введите должность")) {
        printf("Должность: %s\n", book[currentSize].post);
    }
    int emails = 0;
    int links = 0;
    printf("Введите количество почт:\n");
    scanf("%d",&emails);
    if (emails >0)
    {
        for (int i = 0; i<emails; i++)
        {
            scanf("%s", book[currentSize].email[i]);
            book[currentSize].amountOfEmails++;
        }
    }
    
    printf("Введите количество ссылок:\n");
    scanf("%d",&links);
    if (links >0)
    {
        for (int i = 0; i<links; i++)
        {
            scanf("%s", book[currentSize].link[i]);
            book[currentSize].amountOfLinks++;
        }
    }
    return ++currentSize;
}

int ChangePersonInformation(Person* book, int maxSize, int currentSize)
{

}

int RemovePerson(Person* book, int maxSize, int currentSize)
{

}

int SizeOfBook(Person* book, int maxSize, int currentSize)
{

}

int InputOptionalString(char *buffer, int size, const char *prompt) {
    printf("%s (Enter - пропустить): ", prompt);
    fgets(buffer, size, stdin);
    
    buffer[strcspn(buffer, "\n")] = 0;
    
    if(strlen(buffer) == 0) {
        return 0;  
    }
    return 1;  
}

void ShowPersons(Person* book, int currentSize)
{
    for (int i = 0; i<currentSize; i++)
    {
        printf("%d.Имя: %s Фамилия: %s\n",i+1,book[i].firstName,book[i].lastName);

    }
}




void Menu(Person* book, int maxSize)
{
    int currentSize = 0;
    printf("= ТЕЛЕФОННАЯ КНИГА =\n");
    int choice = 0;
    while (choice!=5)
    {
        printf("Введите необходимую операцию:\n1.Вывести список людей\n2.Добавить запись\n3.Изменить запись\n4.Удалить запись\n5.Выход\n-->");
        scanf("%d",&choice);
        switch (choice)
        {
            case 1:
            ShowPersons(book,currentSize);
            break;

            case 2:
            currentSize = AddPerson(book,maxSize,currentSize);
            break;

            case 3:

            break;

            case 4:
            
            break;

            case 5:
            printf("хорошего дня :)\n");
            break;

            default:
            printf("Введена неверная команда.\n");
        }
    }
}
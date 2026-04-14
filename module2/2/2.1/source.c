#include "source.h"
#include <string.h>

int AddPerson(Person* book, int maxSize, int currentSize)
{   
    if (currentSize>=maxSize)
    {
        printf("Книга переполнена, невозможно добавить новую запись.\n");
        return currentSize;
    }
    book[currentSize].fields[5] = 0;
    book[currentSize].fields[6] = 0;
    book[currentSize].fields[7] = 0;

    printf("Добавление записи:\n*-обязательное поле\n");
    printf("Введите имя*:\n->");
    scanf("%s", book[currentSize].firstName);
    book[currentSize].fields[0]++;
    printf("Введите фамилию*:\n->");
    scanf("%s", book[currentSize].lastName);
    book[currentSize].fields[1]++;
    // Очистка буфера для fgets
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    if(InputOptionalString(book[currentSize].patrynomic, MAX_LENGTH, "Введите отчество"))
    {
        book[currentSize].fields[2]++;
    }
    if(InputOptionalString(book[currentSize].placeOfWork, MAX_LENGTH, "Введите место работы"))
    {
        book[currentSize].fields[3]++;
    }
    if(InputOptionalString(book[currentSize].post, MAX_LENGTH, "Введите должность"))
    {
        book[currentSize].fields[4]++;
    }
    
    int phones = 0;
    printf("Введите количество номеров телефонов (максимум 5):\n");
    scanf("%d",&phones);
    if (phones > 0)
    {
        for (int i = 0; i < phones && i < MAX_AMOUNT; i++)
        {
            printf("Введите номер телефона %d\n->", i+1);
            scanf("%s", book[currentSize].phoneNumber[i]);
            book[currentSize].fields[5]++;
        }
    }
    
    int emails = 0;
    printf("Введите количество почт (максимум 5):\n");
    scanf("%d",&emails);
    if (emails > 0)
    {
        for (int i = 0; i < emails && i < MAX_AMOUNT; i++)
        {
            printf("Введите почту номер %d\n->", i+1);
            scanf("%s", book[currentSize].email[i]);
            book[currentSize].fields[6]++;
        }
    }
    
    int links = 0;
    printf("Введите количество ссылок (максимум 5):\n");
    scanf("%d",&links);
    if (links > 0)
    {
        for (int i = 0; i < links && i < MAX_AMOUNT; i++)
        {
            printf("Введите ссылку номер %d\n->", i+1);
            scanf("%s", book[currentSize].link[i]);
            book[currentSize].fields[7]++;
        }
    }
    return ++currentSize;
}

int ChangePersonInformation(Person* book, int currentSize, int personToChange)
{
    if (personToChange > 0 && personToChange <= currentSize)
    {
        int personIndex = personToChange - 1;
        int choice = -1;
        while (choice != 0)
        {
            printf("\nВведите номер поля для замены:\n");
            printf("1.Имя\n2.Фамилия\n3.Отчество\n4.Место работы\n5.Должность\n");
            printf("6.Номера телефонов\n7.Почты\n8.Ссылки\n0.Выход в меню\n->");
            scanf("%d", &choice);
            
            switch (choice)
            {
                case 1:
                    printf("Текущее имя: %s\nВведите новое значение:\n-> ", book[personIndex].firstName);
                    scanf("%s", book[personIndex].firstName);
                    break;
                case 2:
                    printf("Текущая фамилия: %s\nВведите новое значение:\n-> ", book[personIndex].lastName);
                    scanf("%s", book[personIndex].lastName);
                    break;
                case 3:
                    printf("Текущее отчество: %s\nВведите новое значение:\n-> ", book[personIndex].patrynomic);
                    scanf("%s", book[personIndex].patrynomic);
                    break;
                case 4:
                    printf("Текущее место работы: %s\nВведите новое значение:\n-> ", book[personIndex].placeOfWork);
                    scanf("%s", book[personIndex].placeOfWork);
                    break;
                case 5:
                    printf("Текущая должность: %s\nВведите новое значение:\n-> ", book[personIndex].post);
                    scanf("%s", book[personIndex].post);
                    break;
                case 6:
                    printf("Текущее количество телефонов: %d\n", book[personIndex].fields[5]);
                    for (int i = 0; i < book[personIndex].fields[5]; i++)
                    {
                        printf("%d. %s\n", i+1, book[personIndex].phoneNumber[i]);
                    }
                    int tempPhones = 0;
                    printf("Введите новое количество телефонов (максимум %d):\n", MAX_AMOUNT);
                    scanf("%d", &tempPhones);
                    if (tempPhones > MAX_AMOUNT) tempPhones = MAX_AMOUNT;
                    book[personIndex].fields[5] = 0;
                    for (int i = 0; i < tempPhones; i++)
                    {
                        printf("Введите телефон номер %d\n->", i+1);
                        scanf("%s", book[personIndex].phoneNumber[i]);
                        book[personIndex].fields[5]++;
                    }
                    break;
                case 7:
                    printf("Текущее количество почт: %d\n", book[personIndex].fields[6]);
                    for (int i = 0; i < book[personIndex].fields[6]; i++)
                    {
                        printf("%d. %s\n", i+1, book[personIndex].email[i]);
                    }
                    int tempEmails = 0;
                    printf("Введите новое количество почт (максимум %d):\n", MAX_AMOUNT);
                    scanf("%d", &tempEmails);
                    if (tempEmails > MAX_AMOUNT) tempEmails = MAX_AMOUNT;
                    book[personIndex].fields[6] = 0;
                    for (int i = 0; i < tempEmails; i++)
                    {
                        printf("Введите почту номер %d\n->", i+1);
                        scanf("%s", book[personIndex].email[i]);
                        book[personIndex].fields[6]++;
                    }
                    break;
                case 8:
                    printf("Текущее количество ссылок: %d\n", book[personIndex].fields[7]);
                    for (int i = 0; i < book[personIndex].fields[7]; i++)
                    {
                        printf("%d. %s\n", i+1, book[personIndex].link[i]);
                    }
                    int tempLinks = 0;
                    printf("Введите новое количество ссылок (максимум %d):\n", MAX_AMOUNT);
                    scanf("%d", &tempLinks);
                    if (tempLinks > MAX_AMOUNT) tempLinks = MAX_AMOUNT;
                    book[personIndex].fields[7] = 0;
                    for (int i = 0; i < tempLinks; i++)
                    {
                        printf("Введите ссылку номер %d\n->", i+1);
                        scanf("%s", book[personIndex].link[i]);
                        book[personIndex].fields[7]++;
                    }
                    break;
                case 0:
                    printf("Возврат в меню.\n");
                    break;
                default:
                    printf("Введена неверная команда.\n");
                    break;
            }
        }
    }
    else
    {
        printf("Запись с номером %d не найдена.\n", personToChange);
    }
    return currentSize;
}

int RemovePerson(Person* book, int currentSize, int personToRemove)
{
    if (personToRemove > 0 && personToRemove <= currentSize)
    {
        int removeIndex = personToRemove - 1;
        if (removeIndex != currentSize - 1)
        {
            book[removeIndex] = book[currentSize - 1];
        }
        printf("Запись номер %d удалена.\n", personToRemove);
        return --currentSize;
    }
    else
    {
        printf("Запись с номером %d не найдена.\n", personToRemove);
        return currentSize;
    }
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
    if (currentSize == 0)
    {
        printf("Телефонная книга пуста.\n");
        return;
    }
    
    for (int i = 0; i < currentSize; i++)
    {
        printf("\n-------%d-------\n", i+1);
        printf("Имя: %s\n", book[i].firstName);
        printf("Фамилия: %s\n", book[i].lastName);
        
        if (book[i].fields[2] != 0)
        {
            printf("Отчество: %s\n", book[i].patrynomic);
        }
        
        if (book[i].fields[3] != 0)
        {
            printf("Место работы: %s\n", book[i].placeOfWork);
        }
        
        if (book[i].fields[4] != 0)
        {
            printf("Должность: %s\n", book[i].post);
        }
        
        if (book[i].fields[5] != 0)
        {
            printf("Номера телефонов:\n");
            for (int z = 0; z < book[i].fields[5]; z++)
            {
                printf("  %d. %s\n", z+1, book[i].phoneNumber[z]);
            }
        }
        
        if (book[i].fields[6] != 0)
        {
            printf("Почты:\n");
            for (int z = 0; z < book[i].fields[6]; z++)
            {
                printf("  %d. %s\n", z+1, book[i].email[z]);
            }
        }
        
        if (book[i].fields[7] != 0)
        {
            printf("Ссылки:\n");
            for (int z = 0; z < book[i].fields[7]; z++)
            {
                printf("  %d. %s\n", z+1, book[i].link[z]);
            }
        }
    }
}

void Menu(Person* book, int maxSize)
{
    int currentSize = 0;
    printf("= ТЕЛЕФОННАЯ КНИГА =\n");
    int choice = 0;
    currentSize = TestPersons(book,currentSize);
    while (choice != 5)
    {
        printf("\nВведите необходимую операцию:\n");
        printf("1.Вывести список людей\n2.Добавить запись\n3.Изменить запись\n4.Удалить запись\n5.Выход\n-->");
        scanf("%d", &choice);
        
        switch (choice)
        {
            case 1:
                ShowPersons(book, currentSize);
                break;
            case 2:
                currentSize = AddPerson(book, maxSize, currentSize);
                break;
            case 3:
                if (currentSize == 0)
                {
                    printf("Нет записей для изменения.\n");
                    break;
                }
                int whichPersonToChange;
                printf("Введите номер записи для изменения (1-%d):\n->", currentSize);
                scanf("%d", &whichPersonToChange);
                ChangePersonInformation(book, currentSize, whichPersonToChange);
                break;
            case 4:
                if (currentSize == 0)
                {
                    printf("Нет записей для удаления.\n");
                    break;
                }
                int whichPersonToRemove;
                printf("Введите номер записи для удаления (1-%d):\n->", currentSize);
                scanf("%d", &whichPersonToRemove);
                currentSize = RemovePerson(book, currentSize, whichPersonToRemove);
                break;
            case 5:
                printf("Хорошего дня :)\n");
                break;
            default:
                printf("Введена неверная команда.\n");
        }
    }
}

int TestPersons(Person *book, int currentSize)
{
    
    strcpy(book[0].firstName, "Федор");
    strcpy(book[0].lastName, "Достоевский");
    strcpy(book[0].patrynomic, "Михайлович");
    strcpy(book[0].placeOfWork, "Петербургская академия наук");
    strcpy(book[0].post, "Писатель, философ");
    
    book[0].fields[0] = 1;
    book[0].fields[1] = 1;
    book[0].fields[2] = 1;
    book[0].fields[3] = 1;
    book[0].fields[4] = 1;
    
    strcpy(book[0].phoneNumber[0], "+7-123-456-78-90");
    book[0].fields[5] = 1;
    
    strcpy(book[0].email[0], "dostoevsky@example.com");
    book[0].fields[6] = 1;
    
    strcpy(book[0].link[0], "https://ru.wikipedia.org/wiki/Достоевский");
    book[0].fields[7] = 1;
    ++currentSize;

    strcpy(book[1].firstName, "Лев");
    strcpy(book[1].lastName, "Толстой");
    strcpy(book[1].patrynomic, "Николаевич");
    strcpy(book[1].placeOfWork, "Ясная Поляна");
    strcpy(book[1].post, "Писатель, мыслитель");
    
    book[1].fields[0] = 1;
    book[1].fields[1] = 1;
    book[1].fields[2] = 1;
    book[1].fields[3] = 1;
    book[1].fields[4] = 1;
    
    strcpy(book[1].phoneNumber[0], "+7-4872-44-55-66");
    book[1].fields[5] = 1;
    
    strcpy(book[1].email[0], "tolstoy@yasnayapolyana.ru");
    book[1].fields[6] = 1;
    
    strcpy(book[1].link[0], "https://ru.wikipedia.org/wiki/Толстой,_Лев_Николаевич");
    strcpy(book[1].link[1], "https://tolstoy.ru/");
    book[1].fields[7] = 2;
    return ++currentSize;
}
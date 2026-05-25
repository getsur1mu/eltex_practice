#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LENGTH 100
#define MAX_AMOUNT 5
#define MAX_PERSONS 100
#define DATA_FILE "contacts.dat"

typedef struct Person
{
    char firstName[MAX_LENGTH];
    char lastName[MAX_LENGTH];
    char patrynomic[MAX_LENGTH];
    char placeOfWork[MAX_LENGTH];
    char post[MAX_LENGTH];
    char phoneNumber[MAX_AMOUNT][MAX_LENGTH];
    char email[MAX_AMOUNT][MAX_LENGTH];
    char link[MAX_AMOUNT][MAX_LENGTH];
    int fields[8];
} Person;

void ClearInput()
{
    int c;

    while ((c = getchar()) != '\n' && c != EOF);
}

int ReadInt()
{
    int value;

    if (scanf("%d", &value) != 1)
    {
        ClearInput();
        return -1;
    }

    return value;
}

int InputOptionalString(char *buffer, int size, const char *prompt)
{
    printf("%s (Enter - пропустить): ", prompt);

    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return 0;
    }

    buffer[strcspn(buffer, "\n")] = '\0';

    if (strlen(buffer) == 0)
        return 0;

    return 1;
}

int LoadBook(Person *book, int maxSize)
{
    int fd;
    int count = 0;
    ssize_t bytes;

    fd = open(DATA_FILE, O_RDONLY);
    if (fd == -1)
    {
        if (errno != ENOENT)
            perror("open");
        return 0;
    }

    while (count < maxSize)
    {
        bytes = read(fd, &book[count], sizeof(Person));
        if (bytes == 0)
            break;
        if (bytes != sizeof(Person))
        {
            printf("Файл содержит неполную запись, она пропущена.\n");
            break;
        }
        count++;
    }

    close(fd);
    printf("Загружено записей: %d\n", count);
    return count;
}

void SaveBook(Person *book, int currentSize)
{
    int fd;

    fd = open(DATA_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open");
        return;
    }

    for (int i = 0; i < currentSize; i++)
    {
        if (write(fd, &book[i], sizeof(Person)) != sizeof(Person))
        {
            perror("write");
            break;
        }
    }

    close(fd);
    printf("Записей сохранено: %d\n", currentSize);
}

void ShowPersons(Person *book, int currentSize)
{
    if (currentSize == 0)
    {
        printf("Телефонная книга пуста.\n");
        return;
    }

    for (int i = 0; i < currentSize; i++)
    {
        printf("\n-------%d-------\n", i + 1);
        printf("Имя: %s\n", book[i].firstName);
        printf("Фамилия: %s\n", book[i].lastName);

        if (book[i].fields[2] != 0)
            printf("Отчество: %s\n", book[i].patrynomic);
        if (book[i].fields[3] != 0)
            printf("Место работы: %s\n", book[i].placeOfWork);
        if (book[i].fields[4] != 0)
            printf("Должность: %s\n", book[i].post);

        if (book[i].fields[5] != 0)
        {
            printf("Номера телефонов:\n");
            for (int z = 0; z < book[i].fields[5]; z++)
                printf("  %d. %s\n", z + 1, book[i].phoneNumber[z]);
        }

        if (book[i].fields[6] != 0)
        {
            printf("Почты:\n");
            for (int z = 0; z < book[i].fields[6]; z++)
                printf("  %d. %s\n", z + 1, book[i].email[z]);
        }

        if (book[i].fields[7] != 0)
        {
            printf("Ссылки:\n");
            for (int z = 0; z < book[i].fields[7]; z++)
                printf("  %d. %s\n", z + 1, book[i].link[z]);
        }
    }
}

int AddPerson(Person *book, int maxSize, int currentSize)
{
    int phones;
    int emails;
    int links;

    if (currentSize >= maxSize)
    {
        printf("Книга переполнена.\n");
        return currentSize;
    }

    memset(&book[currentSize], 0, sizeof(Person));

    printf("Добавление записи:\n*-обязательное поле\n");
    printf("Введите имя*:\n->");
    scanf("%99s", book[currentSize].firstName);
    book[currentSize].fields[0] = 1;

    printf("Введите фамилию*:\n->");
    scanf("%99s", book[currentSize].lastName);
    book[currentSize].fields[1] = 1;

    ClearInput();

    if (InputOptionalString(book[currentSize].patrynomic, MAX_LENGTH, "Введите отчество"))
        book[currentSize].fields[2] = 1;
    if (InputOptionalString(book[currentSize].placeOfWork, MAX_LENGTH, "Введите место работы"))
        book[currentSize].fields[3] = 1;
    if (InputOptionalString(book[currentSize].post, MAX_LENGTH, "Введите должность"))
        book[currentSize].fields[4] = 1;

    printf("Введите количество номеров телефонов (максимум 5):\n");
    phones = ReadInt();
    if (phones < 0) phones = 0;
    if (phones > MAX_AMOUNT) phones = MAX_AMOUNT;
    for (int i = 0; i < phones; i++)
    {
        printf("Введите номер телефона %d\n->", i + 1);
        scanf("%99s", book[currentSize].phoneNumber[i]);
        book[currentSize].fields[5]++;
    }

    printf("Введите количество почт (максимум 5):\n");
    emails = ReadInt();
    if (emails < 0) emails = 0;
    if (emails > MAX_AMOUNT) emails = MAX_AMOUNT;
    for (int i = 0; i < emails; i++)
    {
        printf("Введите почту номер %d\n->", i + 1);
        scanf("%99s", book[currentSize].email[i]);
        book[currentSize].fields[6]++;
    }

    printf("Введите количество ссылок (максимум 5):\n");
    links = ReadInt();
    if (links < 0) links = 0;
    if (links > MAX_AMOUNT) links = MAX_AMOUNT;
    for (int i = 0; i < links; i++)
    {
        printf("Введите ссылку номер %d\n->", i + 1);
        scanf("%99s", book[currentSize].link[i]);
        book[currentSize].fields[7]++;
    }

    return currentSize + 1;
}

void ChangeString(char *field, int *flag, const char *name)
{
    printf("Введите новое значение поля \"%s\":\n->", name);
    scanf("%99s", field);
    *flag = 1;
}

void ChangePersonInformation(Person *book, int currentSize, int personToChange)
{
    int personIndex = personToChange - 1;
    int choice = -1;

    if (personToChange <= 0 || personToChange > currentSize)
    {
        printf("Запись с номером %d не найдена.\n", personToChange);
        return;
    }

    while (choice != 0)
    {
        printf("\nВведите номер поля для замены:\n");
        printf("1.Имя\n2.Фамилия\n3.Отчество\n4.Место работы\n5.Должность\n");
        printf("6.Номера телефонов\n7.Почты\n8.Ссылки\n0.Выход в меню\n->");
        choice = ReadInt();

        switch (choice)
        {
            case 1:
                ChangeString(book[personIndex].firstName, &book[personIndex].fields[0], "Имя");
                break;
            case 2:
                ChangeString(book[personIndex].lastName, &book[personIndex].fields[1], "Фамилия");
                break;
            case 3:
                ChangeString(book[personIndex].patrynomic, &book[personIndex].fields[2], "Отчество");
                break;
            case 4:
                ChangeString(book[personIndex].placeOfWork, &book[personIndex].fields[3], "Место работы");
                break;
            case 5:
                ChangeString(book[personIndex].post, &book[personIndex].fields[4], "Должность");
                break;
            case 6:
            {
                int phones;
                memset(book[personIndex].phoneNumber, 0, sizeof(book[personIndex].phoneNumber));
                book[personIndex].fields[5] = 0;
                printf("Введите новое количество телефонов (максимум %d):\n", MAX_AMOUNT);
                phones = ReadInt();
                if (phones < 0) phones = 0;
                if (phones > MAX_AMOUNT) phones = MAX_AMOUNT;
                for (int i = 0; i < phones; i++)
                {
                    printf("Введите телефон номер %d\n->", i + 1);
                    scanf("%99s", book[personIndex].phoneNumber[i]);
                    book[personIndex].fields[5]++;
                }
                break;
            }
            case 7:
            {
                int emails;
                memset(book[personIndex].email, 0, sizeof(book[personIndex].email));
                book[personIndex].fields[6] = 0;
                printf("Введите новое количество почт (максимум %d):\n", MAX_AMOUNT);
                emails = ReadInt();
                if (emails < 0) emails = 0;
                if (emails > MAX_AMOUNT) emails = MAX_AMOUNT;
                for (int i = 0; i < emails; i++)
                {
                    printf("Введите почту номер %d\n->", i + 1);
                    scanf("%99s", book[personIndex].email[i]);
                    book[personIndex].fields[6]++;
                }
                break;
            }
            case 8:
            {
                int links;
                memset(book[personIndex].link, 0, sizeof(book[personIndex].link));
                book[personIndex].fields[7] = 0;
                printf("Введите новое количество ссылок (максимум %d):\n", MAX_AMOUNT);
                links = ReadInt();
                if (links < 0) links = 0;
                if (links > MAX_AMOUNT) links = MAX_AMOUNT;
                for (int i = 0; i < links; i++)
                {
                    printf("Введите ссылку номер %d\n->", i + 1);
                    scanf("%99s", book[personIndex].link[i]);
                    book[personIndex].fields[7]++;
                }
                break;
            }
            case 0:
                printf("Возврат в меню.\n");
                break;
            default:
                printf("Введена неверная команда.\n");
                break;
        }
    }
}

int RemovePerson(Person *book, int currentSize, int personToRemove)
{
    int removeIndex = personToRemove - 1;

    if (personToRemove <= 0 || personToRemove > currentSize)
    {
        printf("Запись с номером %d не найдена.\n", personToRemove);
        return currentSize;
    }

    for (int i = removeIndex; i < currentSize - 1; i++)
    {
        book[i] = book[i + 1];
    }

    printf("Запись номер %d удалена.\n", personToRemove);
    return currentSize - 1;
}

void Menu(Person *book, int maxSize)
{
    int currentSize = LoadBook(book, maxSize);
    int choice = 0;

    printf("= ТЕЛЕФОННАЯ КНИГА С ФАЙЛОМ =\n");

    while (choice != 5)
    {
        printf("\nВведите необходимую операцию:\n");
        printf("1.Вывести список людей\n2.Добавить запись\n3.Изменить запись\n4.Удалить запись\n5.Выход\n-->");
        choice = ReadInt();

        switch (choice)
        {
            case 1:
                ShowPersons(book, currentSize);
                break;
            case 2:
                currentSize = AddPerson(book, maxSize, currentSize);
                break;
            case 3:
            {
                int person;
                printf("Введите номер записи для изменения:\n->");
                person = ReadInt();
                ChangePersonInformation(book, currentSize, person);
                break;
            }
            case 4:
            {
                int person;
                printf("Введите номер записи для удаления:\n->");
                person = ReadInt();
                currentSize = RemovePerson(book, currentSize, person);
                break;
            }
            case 5:
                SaveBook(book, currentSize);
                printf("Хорошего дня :)\n");
                break;
            default:
                printf("Введена неверная команда.\n");
                break;
        }
    }
}

int main()
{
    Person book[MAX_PERSONS];

    memset(book, 0, sizeof(book));
    Menu(book, MAX_PERSONS);

    return 0;
}

#include "source.h"

void Menu()
{
    BookNode *head = NULL;
    int choice = 0;

    printf("= ТЕЛЕФОННАЯ КНИГА =\n");
    TestPersons(&head);

    while (choice != 5)
    {
        printf("\nВведите необходимую операцию:\n");
        printf("1.Вывести список людей\n2.Добавить запись\n3.Изменить запись\n4.Удалить запись\n5.Выход\n-->");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                ShowPersons(head);
                break;
            case 2:
                AddPerson(&head);
                break;
            case 3:
            {
                int number;
                printf("Введите номер записи для изменения (1-%d):\n->", CountPersons(head));
                scanf("%d", &number);
                ChangePersonInformation(&head, number);
                break;
            }
            case 4:
            {
                int number;
                printf("Введите номер записи для удаления (1-%d):\n->", CountPersons(head));
                scanf("%d", &number);
                RemovePersonFromList(&head, number);
                break;
            }
            case 5:
                printf("Хорошего дня :)\n");
                break;
            default:
                printf("Введена неверная команда.\n");
                break;
        }
    }

    FreeBook(head);
}

void AddPerson(BookNode **head)
{
    Person person;
    int c;

    memset(&person, 0, sizeof(Person));

    printf("Добавление записи:\n*-обязательное поле\n");
    printf("Введите имя*:\n->");
    scanf("%s", person.firstName);
    person.fields[0] = 1;
    printf("Введите фамилию*:\n->");
    scanf("%s", person.lastName);
    person.fields[1] = 1;

    while ((c = getchar()) != '\n' && c != EOF);

    if (InputOptionalString(person.patrynomic, MAX_LENGTH, "Введите отчество"))
        person.fields[2] = 1;
    if (InputOptionalString(person.placeOfWork, MAX_LENGTH, "Введите место работы"))
        person.fields[3] = 1;
    if (InputOptionalString(person.post, MAX_LENGTH, "Введите должность"))
        person.fields[4] = 1;

    AddPersonToList(head, person);
}

void ChangePersonInformation(BookNode **head, int number)
{
    BookNode *node = GetNodeByIndex(*head, number);
    Person person;

    if (node == NULL)
    {
        printf("Запись не найдена.\n");
        return;
    }

    person = node->person;
    printf("Введите новое имя:\n->");
    scanf("%s", person.firstName);
    printf("Введите новую фамилию:\n->");
    scanf("%s", person.lastName);

    RemovePersonFromList(head, number);
    AddPersonToList(head, person);
}

void ShowPersons(BookNode *head)
{
    int i = 1;

    if (head == NULL)
    {
        printf("Телефонная книга пуста.\n");
        return;
    }

    while (head != NULL)
    {
        printf("\n-------%d-------\n", i);
        printf("Имя: %s\n", head->person.firstName);
        printf("Фамилия: %s\n", head->person.lastName);
        if (head->person.fields[2])
            printf("Отчество: %s\n", head->person.patrynomic);
        if (head->person.fields[3])
            printf("Место работы: %s\n", head->person.placeOfWork);
        if (head->person.fields[4])
            printf("Должность: %s\n", head->person.post);

        head = head->next;
        i++;
    }
}

int InputOptionalString(char *buffer, int size, const char *prompt)
{
    printf("%s (Enter - пропустить): ", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    if (strlen(buffer) == 0)
        return 0;

    return 1;
}

void TestPersons(BookNode **head)
{
    Person person;

    memset(&person, 0, sizeof(Person));
    strcpy(person.firstName, "Федор");
    strcpy(person.lastName, "Достоевский");
    strcpy(person.patrynomic, "Михайлович");
    person.fields[0] = 1;
    person.fields[1] = 1;
    person.fields[2] = 1;
    AddPersonToList(head, person);

    memset(&person, 0, sizeof(Person));
    strcpy(person.firstName, "Лев");
    strcpy(person.lastName, "Толстой");
    strcpy(person.patrynomic, "Николаевич");
    person.fields[0] = 1;
    person.fields[1] = 1;
    person.fields[2] = 1;
    AddPersonToList(head, person);
}

#include "source.h"

void Menu()
{
    TreeNode *root = NULL;
    int choice = 0;
    int changes = 0;

    printf("= ТЕЛЕФОННАЯ КНИГА =\n");
    TestPersons(&root);
    CheckBalance(&root, &changes);

    while (choice != 5)
    {
        printf("\nВведите необходимую операцию:\n");
        printf("1.Вывести список людей\n2.Добавить запись\n3.Изменить запись\n4.Удалить запись\n5.Выход\n-->");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
            {
                int number = 1;
                if (root == NULL)
                    printf("Телефонная книга пуста.\n");
                else
                    ShowPersons(root, &number);
                break;
            }
            case 2:
                AddPerson(&root, &changes);
                break;
            case 3:
            {
                int whichPersonToChange;
                if (root == NULL)
                {
                    printf("Нет записей для изменения.\n");
                    break;
                }
                printf("Введите номер записи для изменения (1-%d):\n->", CountPersons(root));
                scanf("%d", &whichPersonToChange);
                ChangePersonInformation(&root, whichPersonToChange, &changes);
                break;
            }
            case 4:
            {
                int whichPersonToRemove;
                if (root == NULL)
                {
                    printf("Нет записей для удаления.\n");
                    break;
                }
                printf("Введите номер записи для удаления (1-%d):\n->", CountPersons(root));
                scanf("%d", &whichPersonToRemove);
                RemovePerson(&root, whichPersonToRemove, &changes);
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

    FreeTree(root);
}

void AddPerson(TreeNode **root, int *changes)
{
    Person person;
    int c;
    int phones = 0;
    int emails = 0;
    int links = 0;

    memset(&person, 0, sizeof(Person));

    printf("Добавление записи:\n*-обязательное поле\n");
    printf("Введите имя*:\n->");
    scanf("%s", person.firstName);
    person.fields[0]++;

    printf("Введите фамилию*:\n->");
    scanf("%s", person.lastName);
    person.fields[1]++;

    while ((c = getchar()) != '\n' && c != EOF);

    if(InputOptionalString(person.patrynomic, MAX_LENGTH, "Введите отчество"))
        person.fields[2]++;
    if(InputOptionalString(person.placeOfWork, MAX_LENGTH, "Введите место работы"))
        person.fields[3]++;
    if(InputOptionalString(person.post, MAX_LENGTH, "Введите должность"))
        person.fields[4]++;

    printf("Введите количество номеров телефонов (максимум 5):\n");
    scanf("%d",&phones);
    if (phones > MAX_AMOUNT) phones = MAX_AMOUNT;
    for (int i = 0; i < phones; i++)
    {
        printf("Введите номер телефона %d\n->", i+1);
        scanf("%s", person.phoneNumber[i]);
        person.fields[5]++;
    }

    printf("Введите количество почт (максимум 5):\n");
    scanf("%d",&emails);
    if (emails > MAX_AMOUNT) emails = MAX_AMOUNT;
    for (int i = 0; i < emails; i++)
    {
        printf("Введите почту номер %d\n->", i+1);
        scanf("%s", person.email[i]);
        person.fields[6]++;
    }

    printf("Введите количество ссылок (максимум 5):\n");
    scanf("%d",&links);
    if (links > MAX_AMOUNT) links = MAX_AMOUNT;
    for (int i = 0; i < links; i++)
    {
        printf("Введите ссылку номер %d\n->", i+1);
        scanf("%s", person.link[i]);
        person.fields[7]++;
    }

    AddPersonToTree(root, person);
    (*changes)++;
    CheckBalance(root, changes);
}

void AddPersonToTree(TreeNode **root, Person person)
{
    if (*root == NULL)
    {
        *root = CreateNode(person);
        return;
    }

    if (ComparePersons(person, (*root)->person) < 0)
        AddPersonToTree(&(*root)->left, person);
    else
        AddPersonToTree(&(*root)->right, person);
}

TreeNode *CreateNode(Person person)
{
    TreeNode *node = malloc(sizeof(TreeNode));

    if (node == NULL)
        return NULL;

    node->person = person;
    node->left = NULL;
    node->right = NULL;

    return node;
}

int ComparePersons(Person a, Person b)
{
    int result = strcmp(a.lastName, b.lastName);

    if (result == 0)
        result = strcmp(a.firstName, b.firstName);

    return result;
}

void ChangePersonInformation(TreeNode **root, int personToChange, int *changes)
{
    int currentIndex = 1;
    TreeNode *node = GetNodeByIndex(*root, personToChange, &currentIndex);
    Person person;
    int choice = -1;

    if (node == NULL)
    {
        printf("Запись с номером %d не найдена.\n", personToChange);
        return;
    }

    person = node->person;

    while (choice != 0)
    {
        printf("\nВведите номер поля для замены:\n");
        printf("1.Имя\n2.Фамилия\n3.Отчество\n4.Место работы\n5.Должность\n");
        printf("6.Номера телефонов\n7.Почты\n8.Ссылки\n0.Выход в меню\n->");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Текущее имя: %s\nВведите новое значение:\n-> ", person.firstName);
                scanf("%s", person.firstName);
                break;
            case 2:
                printf("Текущая фамилия: %s\nВведите новое значение:\n-> ", person.lastName);
                scanf("%s", person.lastName);
                break;
            case 3:
                printf("Текущее отчество: %s\nВведите новое значение:\n-> ", person.patrynomic);
                scanf("%s", person.patrynomic);
                person.fields[2] = 1;
                break;
            case 4:
                printf("Текущее место работы: %s\nВведите новое значение:\n-> ", person.placeOfWork);
                scanf("%s", person.placeOfWork);
                person.fields[3] = 1;
                break;
            case 5:
                printf("Текущая должность: %s\nВведите новое значение:\n-> ", person.post);
                scanf("%s", person.post);
                person.fields[4] = 1;
                break;
            case 6:
            {
                int tempPhones = 0;
                printf("Введите новое количество телефонов (максимум %d):\n", MAX_AMOUNT);
                scanf("%d", &tempPhones);
                if (tempPhones > MAX_AMOUNT) tempPhones = MAX_AMOUNT;
                person.fields[5] = 0;
                for (int i = 0; i < tempPhones; i++)
                {
                    printf("Введите телефон номер %d\n->", i+1);
                    scanf("%s", person.phoneNumber[i]);
                    person.fields[5]++;
                }
                break;
            }
            case 7:
            {
                int tempEmails = 0;
                printf("Введите новое количество почт (максимум %d):\n", MAX_AMOUNT);
                scanf("%d", &tempEmails);
                if (tempEmails > MAX_AMOUNT) tempEmails = MAX_AMOUNT;
                person.fields[6] = 0;
                for (int i = 0; i < tempEmails; i++)
                {
                    printf("Введите почту номер %d\n->", i+1);
                    scanf("%s", person.email[i]);
                    person.fields[6]++;
                }
                break;
            }
            case 8:
            {
                int tempLinks = 0;
                printf("Введите новое количество ссылок (максимум %d):\n", MAX_AMOUNT);
                scanf("%d", &tempLinks);
                if (tempLinks > MAX_AMOUNT) tempLinks = MAX_AMOUNT;
                person.fields[7] = 0;
                for (int i = 0; i < tempLinks; i++)
                {
                    printf("Введите ссылку номер %d\n->", i+1);
                    scanf("%s", person.link[i]);
                    person.fields[7]++;
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

    *root = RemoveByPerson(*root, node->person);
    AddPersonToTree(root, person);
    (*changes)++;
    CheckBalance(root, changes);
}

void RemovePerson(TreeNode **root, int personToRemove, int *changes)
{
    int currentIndex = 1;
    TreeNode *node = GetNodeByIndex(*root, personToRemove, &currentIndex);

    if (node == NULL)
    {
        printf("Запись с номером %d не найдена.\n", personToRemove);
        return;
    }

    *root = RemoveByPerson(*root, node->person);
    printf("Запись номер %d удалена.\n", personToRemove);
    (*changes)++;
    CheckBalance(root, changes);
}

TreeNode *RemoveByPerson(TreeNode *root, Person person)
{
    TreeNode *temp;

    if (root == NULL)
        return NULL;

    if (ComparePersons(person, root->person) < 0)
        root->left = RemoveByPerson(root->left, person);
    else if (ComparePersons(person, root->person) > 0)
        root->right = RemoveByPerson(root->right, person);
    else
    {
        if (root->left == NULL)
        {
            temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            temp = root->left;
            free(root);
            return temp;
        }

        temp = FindMin(root->right);
        root->person = temp->person;
        root->right = RemoveByPerson(root->right, temp->person);
    }

    return root;
}

TreeNode *FindMin(TreeNode *root)
{
    while (root != NULL && root->left != NULL)
    {
        root = root->left;
    }

    return root;
}

TreeNode *GetNodeByIndex(TreeNode *root, int index, int *currentIndex)
{
    TreeNode *found;

    if (root == NULL)
        return NULL;

    found = GetNodeByIndex(root->left, index, currentIndex);
    if (found != NULL)
        return found;

    if (*currentIndex == index)
        return root;
    (*currentIndex)++;

    return GetNodeByIndex(root->right, index, currentIndex);
}

void ShowPersons(TreeNode *root, int *number)
{
    Person person;

    if (root == NULL)
        return;

    ShowPersons(root->left, number);

    person = root->person;
    printf("\n-------%d-------\n", *number);
    printf("Имя: %s\n", person.firstName);
    printf("Фамилия: %s\n", person.lastName);

    if (person.fields[2] != 0)
        printf("Отчество: %s\n", person.patrynomic);
    if (person.fields[3] != 0)
        printf("Место работы: %s\n", person.placeOfWork);
    if (person.fields[4] != 0)
        printf("Должность: %s\n", person.post);

    if (person.fields[5] != 0)
    {
        printf("Номера телефонов:\n");
        for (int z = 0; z < person.fields[5]; z++)
            printf("  %d. %s\n", z+1, person.phoneNumber[z]);
    }

    if (person.fields[6] != 0)
    {
        printf("Почты:\n");
        for (int z = 0; z < person.fields[6]; z++)
            printf("  %d. %s\n", z+1, person.email[z]);
    }

    if (person.fields[7] != 0)
    {
        printf("Ссылки:\n");
        for (int z = 0; z < person.fields[7]; z++)
            printf("  %d. %s\n", z+1, person.link[z]);
    }

    (*number)++;
    ShowPersons(root->right, number);
}

int CountPersons(TreeNode *root)
{
    if (root == NULL)
        return 0;

    return 1 + CountPersons(root->left) + CountPersons(root->right);
}

int InputOptionalString(char *buffer, int size, const char *prompt)
{
    printf("%s (Enter - пропустить): ", prompt);
    fgets(buffer, size, stdin);

    buffer[strcspn(buffer, "\n")] = 0;

    if(strlen(buffer) == 0)
        return 0;

    return 1;
}

void CheckBalance(TreeNode **root, int *changes)
{
    int count;
    Person *array;
    int index = 0;

    if (*changes < BALANCE_PERIOD)
        return;

    count = CountPersons(*root);
    if (count == 0)
        return;

    array = malloc(sizeof(Person) * count);
    if (array == NULL)
        return;

    FillArray(*root, array, &index);
    FreeTree(*root);
    *root = BuildBalanced(array, 0, count - 1);
    free(array);
    *changes = 0;
    printf("Дерево сбалансировано.\n");
}

void FillArray(TreeNode *root, Person *array, int *index)
{
    if (root == NULL)
        return;

    FillArray(root->left, array, index);
    array[*index] = root->person;
    (*index)++;
    FillArray(root->right, array, index);
}

TreeNode *BuildBalanced(Person *array, int left, int right)
{
    int mid;
    TreeNode *root;

    if (left > right)
        return NULL;

    mid = (left + right) / 2;
    root = CreateNode(array[mid]);

    root->left = BuildBalanced(array, left, mid - 1);
    root->right = BuildBalanced(array, mid + 1, right);

    return root;
}

void FreeTree(TreeNode *root)
{
    if (root == NULL)
        return;

    FreeTree(root->left);
    FreeTree(root->right);
    free(root);
}

void TestPersons(TreeNode **root)
{
    Person person;

    memset(&person, 0, sizeof(Person));
    strcpy(person.firstName, "Федор");
    strcpy(person.lastName, "Достоевский");
    strcpy(person.patrynomic, "Михайлович");
    strcpy(person.placeOfWork, "Петербургская академия наук");
    strcpy(person.post, "Писатель");
    person.fields[0] = 1;
    person.fields[1] = 1;
    person.fields[2] = 1;
    person.fields[3] = 1;
    person.fields[4] = 1;
    strcpy(person.phoneNumber[0], "+7-123-456-78-90");
    person.fields[5] = 1;
    strcpy(person.email[0], "dostoevsky@example.com");
    person.fields[6] = 1;
    AddPersonToTree(root, person);

    memset(&person, 0, sizeof(Person));
    strcpy(person.firstName, "Лев");
    strcpy(person.lastName, "Толстой");
    strcpy(person.patrynomic, "Николаевич");
    strcpy(person.placeOfWork, "Ясная Поляна");
    strcpy(person.post, "Писатель");
    person.fields[0] = 1;
    person.fields[1] = 1;
    person.fields[2] = 1;
    person.fields[3] = 1;
    person.fields[4] = 1;
    strcpy(person.phoneNumber[0], "+7-4872-44-55-66");
    person.fields[5] = 1;
    strcpy(person.link[0], "https://tolstoy.ru/");
    person.fields[7] = 1;
    AddPersonToTree(root, person);
}

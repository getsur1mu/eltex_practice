#include "source.h"

void StartCalculator(char *pluginsDir)
{
    Operation operations[MAX_OPERATIONS];
    Operation *currentOperation;
    int count;
    int choice = -1;
    double a;
    double b;

    count = LoadOperations(pluginsDir, operations);
    if (count == 0)
    {
        printf("Операции не найдены.\n");
        return;
    }

    printf("== КАЛЬКУЛЯТОР ==\n\n");
    printf("Введите аргумент 1:\n -> ");
    while (!ReadDouble(&a))
        printf("Введите число:\n -> ");

    while (choice != 0)
    {
        printf("Выберите операцию:\n1: [+] (Сложение)\n2: [-] (Вычитание)\n3: [*] (Умножение)\n4: [/] (Деление)\n5: [^] (Возведение в целую степень)\n0: Выход\n -> ");
        if (!ReadInt(&choice))
        {
            printf("Введите номер операции.\n");
            continue;
        }

        if (choice == 0)
            break;

        switch (choice)
        {
            case 1:
                currentOperation = FindOperation(operations, count, "+");
                break;
            case 2:
                currentOperation = FindOperation(operations, count, "-");
                break;
            case 3:
                currentOperation = FindOperation(operations, count, "*");
                break;
            case 4:
                currentOperation = FindOperation(operations, count, "/");
                break;
            case 5:
                currentOperation = FindOperation(operations, count, "^");
                break;
            default:
                currentOperation = NULL;
                break;
        }

        if (currentOperation == NULL)
        {
            printf("Выбрана неверная операция\n");
            continue;
        }

        printf("Введите аргумент 2:\n -> ");
        while (!ReadDouble(&b))
            printf("Введите число:\n -> ");

        a = currentOperation->func(a, b);
        printf("-----------------\n%f\n-----------------\n", a);
    }

    CloseOperations(operations, count);
    printf("Хорошего дня :)\n");
}

int ReadInt(int *value)
{
    char input[100];
    char *end;
    long number;

    scanf("%99s", input);
    number = strtol(input, &end, 10);

    if (*end != '\0')
        return 0;

    *value = number;
    return 1;
}

int ReadDouble(double *value)
{
    char input[100];
    char *end;
    double number;

    scanf("%99s", input);
    number = strtod(input, &end);

    if (*end != '\0')
        return 0;

    *value = number;
    return 1;
}

Operation *FindOperation(Operation *operations, int count, char *name)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(operations[i].name, name) == 0)
            return &operations[i];
    }

    return NULL;
}

int LoadOperations(char *pluginsDir, Operation *operations)
{
    DIR *dir;
    struct dirent *entry;
    char path[MAX_PATH];
    void *handle;
    NameFunc nameFunc;
    OperationFunc operationFunc;
    int count = 0;

    dir = opendir(pluginsDir);
    if (dir == NULL)
    {
        printf("Не удалось открыть каталог: %s\n", pluginsDir);
        return 0;
    }

    while ((entry = readdir(dir)) != NULL && count < MAX_OPERATIONS)
    {
        if (!IsSharedLibrary(entry->d_name))
            continue;

        snprintf(path, MAX_PATH, "%s/%s", pluginsDir, entry->d_name);
        handle = dlopen(path, RTLD_NOW);
        if (handle == NULL)
            continue;

        nameFunc = (NameFunc)dlsym(handle, "operation_name");
        operationFunc = (OperationFunc)dlsym(handle, "operation_call");
        if (nameFunc == NULL || operationFunc == NULL)
        {
            dlclose(handle);
            continue;
        }

        operations[count].handle = handle;
        strcpy(operations[count].name, nameFunc());
        operations[count].func = operationFunc;
        count++;
    }

    closedir(dir);
    return count;
}

void CloseOperations(Operation *operations, int count)
{
    for (int i = 0; i < count; i++)
    {
        dlclose(operations[i].handle);
    }
}

int IsSharedLibrary(char *filename)
{
    int len = strlen(filename);

    if (len < 3)
        return 0;

    if (strcmp(filename + len - 3, ".so") == 0)
        return 1;

    return 0;
}

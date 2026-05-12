#include "source.h"

void StartMascCalc(int argc, char *argv[])
{
    int choice = -1;
    mode_t mask = 0;
    char input[MAX_INPUT];

    printf("== КАЛЬКУЛЯТОР ПРАВ ==\n\n");

    if (argc > 1)
    {
        if (GetMasc(argv[1], &mask) == 0 || CalcMasc(argv[1], &mask) == 0)
        {
            ShowMasc(mask);
            ChangeMascMenu(&mask);
        }
        else
        {
            printf("Не получилось прочитать аргумент: %s\n", argv[1]);
        }
        return;
    }

    while (choice != 0)
    {
        printf("Выберите действие:\n");
        printf("1. Ввести права доступа\n");
        printf("2. Ввести имя файла\n");
        printf("0. Выход\n -> ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Введите права (например 644 или rw-r--r--):\n -> ");
                scanf("%s", input);
                if (CalcMasc(input, &mask) == 0)
                {
                    ShowMasc(mask);
                    ChangeMascMenu(&mask);
                }
                else
                {
                    printf("Неверный формат прав.\n");
                }
                break;
            case 2:
                printf("Введите имя файла:\n -> ");
                scanf("%s", input);
                if (GetMasc(input, &mask) == 0)
                {
                    ShowMasc(mask);
                    ChangeMascMenu(&mask);
                }
                else
                {
                    printf("Файл не найден.\n");
                }
                break;
            case 0:
                printf("Хорошего дня :)\n");
                break;
            default:
                printf("Введена неверная команда.\n");
                break;
        }
    }
}

int GetMasc(char *filename, mode_t *mask)
{
    struct stat fileStat;

    if (stat(filename, &fileStat) != 0)
    {
        return -1;
    }

    *mask = fileStat.st_mode & 0777;
    return 0;
}

int CalcMasc(char *permissions, mode_t *mask)
{
    int len = strlen(permissions);
    int start = 0;
    mode_t tempMask = 0;

    if (len == 4 && permissions[0] == '0')
    {
        permissions++;
        len = 3;
    }

    if (len == 3)
    {
        for (int i = 0; i < 3; i++)
        {
            if (permissions[i] < '0' || permissions[i] > '7')
                return -1;

            tempMask = (tempMask << 3) + (permissions[i] - '0');
        }
        *mask = tempMask;
        return 0;
    }

    if (len == 10)
        start = 1;

    if (len != RIGHTS_LEN && len != RIGHTS_LEN + 1)
        return -1;

    for (int i = 0; i < RIGHTS_LEN; i++)
    {
        char c = permissions[start + i];
        int pos = RIGHTS_LEN - 1 - i;

        if (i % 3 == 0 && c == 'r')
            tempMask |= 1 << pos;
        else if (i % 3 == 1 && c == 'w')
            tempMask |= 1 << pos;
        else if (i % 3 == 2 && c == 'x')
            tempMask |= 1 << pos;
        else if (c != '-')
            return -1;
    }

    *mask = tempMask;
    return 0;
}

void ShowMasc(mode_t mask)
{
    char letters[RIGHTS_LEN + 1];
    char bits[RIGHTS_LEN + 1];
    char symbols[] = "rwx";

    for (int i = 0; i < RIGHTS_LEN; i++)
    {
        int bit = RIGHTS_LEN - 1 - i;
        if (mask & (1 << bit))
        {
            letters[i] = symbols[i % 3];
            bits[i] = '1';
        }
        else
        {
            letters[i] = '-';
            bits[i] = '0';
        }
    }

    letters[RIGHTS_LEN] = '\0';
    bits[RIGHTS_LEN] = '\0';

    printf("-----------------\n");
    printf("Буквенно: %s\n", letters);
    printf("Цифрами: %o\n", mask & 0777);
    printf("Битами: %s\n", bits);
    printf("-----------------\n");
}

void ShowSmallMasc(char *name, mode_t mask)
{
    char bits[RIGHTS_LEN + 1];

    for (int i = 0; i < RIGHTS_LEN; i++)
    {
        int bit = RIGHTS_LEN - 1 - i;
        if (mask & (1 << bit))
            bits[i] = '1';
        else
            bits[i] = '0';
    }

    bits[RIGHTS_LEN] = '\0';
    printf("%s: %03o (%s)\n", name, mask & 0777, bits);
}

void ChangeMascMenu(mode_t *mask)
{
    char command[MAX_INPUT];

    while (1)
    {
        printf("Введите команду изменения прав (пример 777, u+x, g-w, o=rx, a+rwx) или 0:\n -> ");
        scanf("%s", command);

        if (strcmp(command, "0") == 0)
            break;

        ApplyCommand(mask, command);
        ShowMasc(*mask);
    }
}

void ApplyCommand(mode_t *mask, char *command)
{
    char parts[MAX_INPUT];
    char *token;
    mode_t newMask;
    mode_t oldMask;

    strcpy(parts, command);
    token = strtok(parts, ",");

    while (token != NULL)
    {
        if (CalcMasc(token, &newMask) == 0)
        {
            oldMask = *mask;
            *mask = newMask;
            ShowRule(oldMask, newMask, *mask, 0777, '=');
        }
        else if (ParceString(mask, token) != 0)
        {
            printf("Команда не понята: %s\n", token);
        }
        token = strtok(NULL, ",");
    }
}

int ParceString(mode_t *mask, char *arguments)
{
    int i = 0;
    int who = 0;
    int rights = 0;
    char action;
    mode_t clearMask = 0;
    mode_t oldMask = *mask;

    while (arguments[i] != '+' && arguments[i] != '-' && arguments[i] != '=')
    {
        if (arguments[i] == '\0')
            return -1;

        if (arguments[i] == 'u')
            who |= 0700;
        else if (arguments[i] == 'g')
            who |= 0070;
        else if (arguments[i] == 'o')
            who |= 0007;
        else if (arguments[i] == 'a')
            who |= 0777;
        else
            return -1;
        i++;
    }

    if (who == 0)
        who = 0777;

    action = arguments[i];
    i++;

    for (; arguments[i] != '\0'; i++)
    {
        if (arguments[i] == 'r')
            rights |= 0444;
        else if (arguments[i] == 'w')
            rights |= 0222;
        else if (arguments[i] == 'x')
            rights |= 0111;
        else
            return -1;
    }

    rights &= who;

    if (action == '+')
    {
        *mask |= rights;
        ShowRule(oldMask, rights, *mask, 0, action);
    }
    else if (action == '-')
    {
        *mask &= ~rights;
        ShowRule(oldMask, rights, *mask, 0, action);
    }
    else if (action == '=')
    {
        clearMask = who;
        *mask &= ~clearMask;
        *mask |= rights;
        ShowRule(oldMask, rights, *mask, clearMask, action);
    }

    return 0;
}

void ShowRule(mode_t oldMask, mode_t commandMask, mode_t resultMask, mode_t clearMask, char action)
{
    ShowSmallMasc("Изначальная маска", oldMask);
    ShowSmallMasc("Маска из команды", commandMask);

    if (action == '+')
        printf("Правило: %03o | %03o = %03o\n", oldMask & 0777, commandMask & 0777, resultMask & 0777);
    else if (action == '-')
        printf("Правило: %03o & ~%03o = %03o\n", oldMask & 0777, commandMask & 0777, resultMask & 0777);
    else if (action == '=' && clearMask == 0777)
        printf("Правило: новая маска = %03o\n", commandMask & 0777);
    else if (action == '=')
        printf("Правило: (%03o & ~%03o) | %03o = %03o\n", oldMask & 0777, clearMask & 0777, commandMask & 0777, resultMask & 0777);
}


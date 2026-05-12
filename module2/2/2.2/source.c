#include "source.h"

void StartCalculator()
{
    int choice = -1;
    double a,b;
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
        if (choice ==0)
        {
            break;
        }
        printf("Введите аргумент 2:\n -> ");
        while (!ReadDouble(&b))
            printf("Введите число:\n -> ");
    
        switch (choice)
        {
            case 1:
            Add(&a,b);
            break;
            
            case 2:
            Sub(&a,b);
            break;

            case 3:
            Mul(&a,b);
            break;

            case 4:
            Div(&a,b);
            break;

            case 5:
            Exp(&a,b);
            break;



            default:
            printf("Выбрана неверная операция\n");
            break;
        }

        printf("-----------------\n%f\n-----------------\n",a);

    }
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

void Add(double* a, double b)
{
    *a+=b;
}

void Sub(double* a, double b)
{
    *a-=b;
}

void Mul(double* a, double b)
{
    *a*=b;
}

void Div(double* a, double b)
{
    if (b!=0)
    {
        *a/=b;
    } else {
        printf("Ошибка: деление на ноль\n");
    }
    
}

void Exp(double* a, double b)
{
    if (b==0)
    {
        *a = 1;
        return;
    }

    int exp = (int)b;
    double temp = 1.0;
    double base = *a;

    for (int i = 0; i < abs(exp); i++)
    {
        temp *=base;
    }

    if (exp < 0)
    {
        temp = 1.0 / temp;
    }

    *a = temp;
}

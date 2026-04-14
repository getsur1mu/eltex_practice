#include "source.h"
#include <stdlib.h>
void StartCalculator()
{
    int choice = -1;
    double a,b;
    printf("== КАЛЬКУЛЯТОР ==\n\n");

    printf("Введите аргумент 1:\n -> ");
    scanf("%lf",&a);

    while (choice != 0)
    {
        printf("Выберите операцию:\n1: [+] (Сложение)\n2: [-] (Вычитание)\n3: [*] (Умножение)\n4: [/] (Деление)\n5: [^] (Возведение в целую степень)\n0: Выход\n -> ");
        scanf("%d",&choice);
        if (choice ==0)
        {
            break;
        }
        printf("Введите аргумент 2:\n -> ");
        scanf("%lf",&b);
    
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
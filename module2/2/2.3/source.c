#include "source.h"
#include <stdlib.h>
void StartCalculator()
{
    int choice = -1;
    double a,b;

    OperationsList mainArray;

    mainArray.array = malloc(sizeof(Operation) * 2);
    mainArray.arrayCapacity = 2;
    mainArray.arrayLength=0;

    printf("== КАЛЬКУЛЯТОР+ ==\n\n");

    // Динамическое добавление функций
    AddCommand(Add,&mainArray);
    AddCommand(Sub,&mainArray);
    AddCommand(Mul,&mainArray);
    AddCommand(Div,&mainArray);
    AddCommand(Exp,&mainArray);

    printf("Введите аргумент 1:\n -> ");
    scanf("%lf",&a);

    while (choice != 0)
    {
        printf("Выберите операцию:\n1: [+] (Сложение)\n2: [-] (Вычитание)\n3: [*] (Умножение)\n4: [/] (Деление)\n5: [^] (Возведение в целую степень)\n0: Выход\n -> ");
        scanf("%d",&choice);
        if (choice == 0)
        {
            break;
        }
        printf("Введите аргумент 2:\n -> ");
        scanf("%lf",&b);
        
        mainArray.array[choice-1](&a,b); // Вызов функции по указателю

        printf("-----------------\n%f\n-----------------\n",a);

    }

    printf("Хорошего дня :)\n");
    //Освобождение памяти
    free(mainArray.array);
}

void AddCommand(Operation op, OperationsList* opList)
{   
    // Обработка переполнения массива
    if (opList->arrayCapacity== opList->arrayLength)
    {   
        Operation* newArray = malloc(sizeof(Operation)*opList->arrayCapacity*2);

        // Ошибка если не удалось выделить память
        if (!newArray)return;

        for (int i = 0; i<opList->arrayLength; i++)
        {
            printf("----Выделение памяти----\n");
            newArray[i]=opList->array[i];
        }

        opList->array=newArray;
        opList->arrayCapacity*2;
    }   
    opList->array[opList->arrayLength]=op;
    opList->arrayLength++;
}



void Add(double* a, double b)   // [+]
{
    *a+=b;
}

void Sub(double* a, double b)   // [-]
{
    *a-=b;
}

void Mul(double* a, double b)   // [*]
{
    *a*=b;
}

void Div(double* a, double b)   // [/]
{
    if (b!=0)
    {
        *a/=b;
    } else {
        printf("Ошибка: деление на ноль\n");
    }
    
}

void Exp(double* a, double b)   // [^]
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
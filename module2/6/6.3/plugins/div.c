#include "../operation.h"
#include <stdio.h>

const char *operation_name()
{
    return "/";
}

double operation_call(double a, double b)
{
    if (b == 0)
    {
        printf("Ошибка: деление на ноль\n");
        return a;
    }

    return a / b;
}

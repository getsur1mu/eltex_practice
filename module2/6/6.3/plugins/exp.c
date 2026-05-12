#include "../operation.h"
#include <stdlib.h>

const char *operation_name()
{
    return "^";
}

double operation_call(double a, double b)
{
    int exp = (int)b;
    double result = 1;

    if (exp == 0)
        return 1;

    for (int i = 0; i < abs(exp); i++)
        result *= a;

    if (exp < 0)
        result = 1.0 / result;

    return result;
}

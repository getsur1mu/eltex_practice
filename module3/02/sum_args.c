#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    double sum = 0;

    if (argc < 2)
    {
        printf("Использование: sum_args <числа>\n");
        return 0;
    }

    for (int i = 1; i < argc; i++)
    {
        sum += atof(argv[i]);
    }

    printf("Сумма: %g\n", sum);
    return 0;
}

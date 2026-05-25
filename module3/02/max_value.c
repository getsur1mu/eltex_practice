#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    double max;

    if (argc < 2)
    {
        printf("Использование: max_value <числа>\n");
        return 0;
    }

    max = atof(argv[1]);
    for (int i = 2; i < argc; i++)
    {
        double value = atof(argv[i]);
        if (value > max)
            max = value;
    }

    printf("Максимум: %g\n", max);
    return 0;
}

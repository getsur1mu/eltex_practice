#include "source.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
        StartCalculator(argv[1]);
    else
        StartCalculator("plugins");

    return 0;
}

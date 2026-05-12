#ifndef OPERATION_H
#define OPERATION_H

typedef double (*OperationFunc)(double a, double b);

const char *operation_name();
double operation_call(double a, double b);

#endif

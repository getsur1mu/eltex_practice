#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>

void StartCalculator();
int ReadInt(int *value);
int ReadDouble(double *value);

typedef void (*Operation)(double*, double); // Указатель на функцию

typedef struct OperationsList{ //Структура хранит информацию по массиву, и сам массив.
    Operation* array;
    int arrayLength;
    int arrayCapacity;
}OperationsList;


void AddCommand(Operation op, OperationsList* opList);



void Add(double* a, double b);
void Sub(double* a, double b);
void Mul(double* a, double b);
void Div(double* a, double b);
void Exp(double* a, double b);



#endif

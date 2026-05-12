#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>

void StartCalculator();
int ReadInt(int *value);
int ReadDouble(double *value);

void Add(double* a, double b);
void Sub(double* a, double b);
void Mul(double* a, double b);
void Div(double* a, double b);
void Exp(double* a, double b);



#endif

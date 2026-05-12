#ifndef SOURCE_H
#define SOURCE_H

#include "operation.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OPERATIONS 20
#define MAX_PATH 1024

typedef const char *(*NameFunc)();

typedef struct Operation{
    void *handle;
    char name[20];
    OperationFunc func;
}Operation;

void StartCalculator(char *pluginsDir);
int LoadOperations(char *pluginsDir, Operation *operations);
void CloseOperations(Operation *operations, int count);
int IsSharedLibrary(char *filename);
Operation *FindOperation(Operation *operations, int count, char *name);
int ReadInt(int *value);
int ReadDouble(double *value);

#endif

#ifndef SOURCE_H
#define SOURCE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define RIGHTS_LEN 9
#define MAX_INPUT 256

void StartMascCalc(int argc, char *argv[]);
int ParceString(mode_t *mask, char *arguments);
int CalcMasc(char *permissions, mode_t *mask);
int GetMasc(char *filename, mode_t *mask);
void ShowMasc(mode_t mask);
void ShowSmallMasc(char *name, mode_t mask);
void ChangeMascMenu(mode_t *mask);
void ApplyCommand(mode_t *mask, char *command);
void ShowRule(mode_t oldMask, mode_t commandMask, mode_t resultMask, mode_t clearMask, char action);

#endif

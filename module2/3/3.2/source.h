#ifndef SOURCE_H
#define SOURCE_H

#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void StartNetwork(int argc, char *argv[]);
int ParseIp(char *str, uint32_t *ip);
uint32_t GenerateIp();
void PrintIp(uint32_t ip);
int IsLocalNetwork(uint32_t gateway, uint32_t mask, uint32_t ip);

#endif

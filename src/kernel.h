#ifndef kernel_H
#define kernel_H

#include "pcb.h"
#include "readyQueue.h"

int getSchedulingPolicyNo(char* policy);

char* kernelInit(const char *filename); 
int scheduler();

#endif

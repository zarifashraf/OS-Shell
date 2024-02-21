#ifndef CPU_H
#define CPU_H

#include "pcb.h"

/*
 * IP: Instruction pointer to shell memory | IP = 101 means CPU is executing the 101th line in shellmemory.
 * IR: Instruction Register - stores the line of code CPU is executing.
 * time_slice: lines of code to be run before termination or switching
 */
typedef struct
{
    int IP;
    char IR[1000];
    int time_slice;
}CPU;


int getCPU_IP();    // cpu_get_ip
void freeCPU();
void loadPCB_toCPU(int PC);
int callCPU(int line_limit, int end);   //cpu_run

int runningCPU(PCB *aPCB);

#endif

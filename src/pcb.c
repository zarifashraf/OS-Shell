#include "pcb.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

PCB* makePCB(int start, int end, char* pid){
    
    PCB * newPCB = malloc(sizeof(PCB));
    newPCB->pid = pid;
    newPCB->PC = start;
    newPCB->start  = start;
    newPCB->end = end;
    newPCB->job_length_score = 1+end-start;

    for(int i = 0; i < 100; i++) {
        newPCB-> pageTable[i] = -1;
    }
    
    newPCB->numOfpages = 0;
    newPCB->index_init_pt = 0;
    newPCB->fs_index = 0;
    newPCB->pointer = 0;
    
    return newPCB;
}
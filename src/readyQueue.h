#include "pcb.h"

#include <stdbool.h>

#ifndef readyQueue_H
#define readyQueue_H

#define READYQUEUE_SIZE 10
#define MAX_INT 2147483646

extern PCB* my_readyQueue[READYQUEUE_SIZE];

void readyQueue_init();
void enqueueFront(PCB *newPCB);
void enqueueEnd(PCB *newPCB);
PCB readyQueuePop(int index, bool inPlace);

void clean_readyQueue();    // ready-queue-empty
void delete_ReadyQueue();

bool isReadyQueueEmpty();
void terminateTaskByIndex(int i);

PCB* get_readyQueueAt(int index);
void print_readyQueue();

#endif

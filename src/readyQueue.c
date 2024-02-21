#include "pcb.h"
#include "readyQueue.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "util.h"
#include "mmu.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

PCB* my_readyQueue[READYQUEUE_SIZE]; 

void readyQueue_init()
{
    for (size_t i = 0; i < READYQUEUE_SIZE; ++i)
    {
        my_readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        (*my_readyQueue[i]).PC = -1;
        (*my_readyQueue[i]).start = -1;
        (*my_readyQueue[i]).end = -1;
        (*my_readyQueue[i]).pid = NULL;
        for(int j = 0; j < 100; j++) {
            (*my_readyQueue[i]).pageTable[j] = -1;
        }
        (*my_readyQueue[i]).index_init_pt = 0;
        (*my_readyQueue[i]).fs_index = 0;
        (*my_readyQueue[i]).pointer = 0;
        (*my_readyQueue[i]).numOfpages = 0;
        (*my_readyQueue[i]).job_length_score = -1;
    }
}

void enqueueFront(PCB *pPCB){
    for (size_t i = READYQUEUE_SIZE-1; i > 0; i--){
        (*my_readyQueue[i]).PC = (*my_readyQueue[i-1]).PC;
        (*my_readyQueue[i]).start = (*my_readyQueue[i-1]).start;
        (*my_readyQueue[i]).end = (*my_readyQueue[i-1]).end;
        memcpy((*my_readyQueue[i]).pageTable, (*my_readyQueue[i-1]).pageTable, sizeof (*my_readyQueue[i-1]).pageTable);
        (*my_readyQueue[i]).index_init_pt = (*my_readyQueue[i-1]).index_init_pt;
        (*my_readyQueue[i]).fs_index = (*my_readyQueue[i-1]).fs_index;
        (*my_readyQueue[i]).pointer = (*my_readyQueue[i-1]).pointer;
        (*my_readyQueue[i]).fileName = (*my_readyQueue[i-1]).fileName;
        (*my_readyQueue[i]).numOfpages = (*my_readyQueue[i-1]).numOfpages;
        (*my_readyQueue[i]).pid = (*my_readyQueue[i-1]).pid;
        (*my_readyQueue[i]).job_length_score = (*my_readyQueue[i-1]).job_length_score;
    }

    (*my_readyQueue[0]).PC = (*pPCB).PC;
    (*my_readyQueue[0]).start = (*pPCB).start;
    (*my_readyQueue[0]).end = (*pPCB).end;
    memcpy((*my_readyQueue[0]).pageTable, (*pPCB).pageTable, sizeof (*pPCB).pageTable);
    (*my_readyQueue[0]).index_init_pt = (*pPCB).index_init_pt;
    (*my_readyQueue[0]).fs_index = (*pPCB).fs_index;
    (*my_readyQueue[0]).pointer = (*pPCB).pointer;
    (*my_readyQueue[0]).numOfpages = (*pPCB).numOfpages;
    (*my_readyQueue[0]).pid = (*pPCB).pid;
    (*my_readyQueue[0]).job_length_score = (*pPCB).job_length_score;
}

void enqueueEnd(PCB *pPCB)
{
    for(int i = 0; i < READYQUEUE_SIZE; i++){
        if ( (*my_readyQueue[i]).start == -1 ){
            (*my_readyQueue[i]).PC = (*pPCB).PC;
            (*my_readyQueue[i]).start = (*pPCB).start;
            (*my_readyQueue[i]).end = (*pPCB).end;
            (*my_readyQueue[i]).pid = (*pPCB).pid;
            memcpy((*my_readyQueue[i]).pageTable, (*pPCB).pageTable, sizeof (*pPCB).pageTable);
            (*my_readyQueue[i]).index_init_pt = (*pPCB).index_init_pt;
            (*my_readyQueue[i]).fs_index = (*pPCB).fs_index;
            (*my_readyQueue[i]).pointer = (*pPCB).pointer;
            (*my_readyQueue[i]).fileName = (*pPCB).fileName;
            (*my_readyQueue[i]).numOfpages = (*pPCB).numOfpages;
            (*my_readyQueue[i]).job_length_score = (*pPCB).job_length_score;
            break;
        }
    }
}

PCB readyQueuePop(int index, bool inPlace)
{
    PCB head = (*my_readyQueue[index]);
    if(inPlace){
        for (size_t i = index+1; i < READYQUEUE_SIZE; i++){
            (*my_readyQueue[i-1]).PC = (*my_readyQueue[i]).PC;
            (*my_readyQueue[i-1]).start = (*my_readyQueue[i]).start;
            (*my_readyQueue[i-1]).end = (*my_readyQueue[i]).end;
            (*my_readyQueue[i-1]).pid = (*my_readyQueue[i]).pid;
            memcpy((*my_readyQueue[i-1]).pageTable, (*my_readyQueue[i]).pageTable, sizeof (*my_readyQueue[i]).pageTable);
            (*my_readyQueue[i-1]).fs_index = (*my_readyQueue[i]).fs_index;
            (*my_readyQueue[i-1]).index_init_pt = (*my_readyQueue[i]).index_init_pt;
            (*my_readyQueue[i-1]).pointer = (*my_readyQueue[i]).pointer;
            (*my_readyQueue[i-1]).fileName = (*my_readyQueue[i]).fileName;
            (*my_readyQueue[i-1]).numOfpages = (*my_readyQueue[i]).numOfpages;
            (*my_readyQueue[i-1]).job_length_score = (*my_readyQueue[i]).job_length_score;
        }
        (*my_readyQueue[READYQUEUE_SIZE-1]).PC = -1;
        (*my_readyQueue[READYQUEUE_SIZE-1]).start = -1;
        (*my_readyQueue[READYQUEUE_SIZE-1]).end = -1;
        for(int j = 0; j < 100; j++) {
            (*my_readyQueue[READYQUEUE_SIZE-1]).pageTable[j] = -1;
        }
        (*my_readyQueue[READYQUEUE_SIZE-1]).index_init_pt = 0;
        (*my_readyQueue[READYQUEUE_SIZE-1]).fs_index = 0;
        (*my_readyQueue[READYQUEUE_SIZE-1]).pointer = 0;
        (*my_readyQueue[READYQUEUE_SIZE-1]).fileName = NULL;
        (*my_readyQueue[READYQUEUE_SIZE-1]).numOfpages = 0;
        (*my_readyQueue[READYQUEUE_SIZE-1]).pid = NULL;
        (*my_readyQueue[READYQUEUE_SIZE-1]).job_length_score = -1;
    }
    return head;
}

void clean_readyQueue(){
    for (size_t i = 0; i < READYQUEUE_SIZE; ++i)
    {
        (*my_readyQueue[i]).PC = -1;
        (*my_readyQueue[i]).start = -1;
        (*my_readyQueue[i]).end = -1;
        (*my_readyQueue[i]).pid = NULL;
        for(int j = 0; j < 100; j++) {
            (*my_readyQueue[i]).pageTable[j] = -1;
        }
        (*my_readyQueue[i]).index_init_pt = 0;
        (*my_readyQueue[i]).fs_index = 0;
        (*my_readyQueue[i]).pointer = 0;
        (*my_readyQueue[i]).numOfpages = 0;
        (*my_readyQueue[i]).job_length_score = -1;
    }
}

void delete_readyQueue()
{
    for (size_t i = 0; i < READYQUEUE_SIZE; ++i)
    {
        free(my_readyQueue[i]);
    }
}

bool isReadyQueueEmpty(){
    for (size_t i = 0; i < READYQUEUE_SIZE; ++i)
    {
        if((*my_readyQueue[i]).start != -1){
            return false;
        }  
    }
    return true;
}

void terminateTaskByIndex(int i){
    (*my_readyQueue[i]).start = -1; 
    (*my_readyQueue[i]).end = -1; 
    (*my_readyQueue[i]).PC = -1; 
    (*my_readyQueue[i]).pid = NULL;
    for(int j = 0; j < 100; j++) {
        (*my_readyQueue[i]).pageTable[j] = -1;
    }
    (*my_readyQueue[i]).index_init_pt = 0;
    (*my_readyQueue[i]).fs_index = 0;
    (*my_readyQueue[i]).pointer = 0;
    (*my_readyQueue[i]).numOfpages = 0;
    (*my_readyQueue[i]).job_length_score = -1;
}

PCB* get_readyQueueAt(int index) {
    return my_readyQueue[index];
}

void print_readyQueue() {
    for(int i = 0; i < READYQUEUE_SIZE; i++) {
        PCB* cur = get_readyQueueAt(i);
        // printf("file: %s\t", cur->fileName);
        printf("num_lines: %d\t", cur->numOfpages);
    }
    printf("\n");
}
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


char* kernelInit(const char *filename){
    FILE* fp;
    
    char* errorCode = "";
    int* start = (int*)malloc(sizeof(int));
    int* end = (int*)malloc(sizeof(int));
    
    
    char* newfile = (char*) loadScript((char*)filename);

    if(fp == NULL){
        errorCode = "11"; 
        return errorCode;
    }

    //generate a random ID as file ID
    char* fileID = (char*)malloc(32);
    sprintf(fileID, "%d", rand());

    PCB* newPCB = makePCB(*start,*end,fileID);
    
    newPCB->fileName = strdup(newfile);
    newPCB->job_length_score = 1 + *end - *start;
    

    enqueueEnd(newPCB);

    return newfile;

}

int getSchedulingPolicyNo(char* policy){
    if(strcmp("FCFS",policy)==0){
        return 0;
    }
    else if(strcmp("SJF",policy)==0){
        return 1;
    }
    else if(strcmp("RR",policy)==0){
        return 2;
    }
    else if(strcmp("AGING",policy)==0){
        return 3;
    }
    else{
        
        return 15;  //error code 15
    }
}

/*
 * Variable:  schedulingPolicy 
 * --------------------
 * 0: FCFS
 * 1: SJF
 * 2: RR
 * 3: AGING
 */
int scheduler(int policyNumber){
    int errorCode = 0;

    int cpu_quanta_per_program = 2;

    
    if( policyNumber == 0 || policyNumber == 1 ){
        cpu_quanta_per_program = MAX_INT;
    }else if(policyNumber == 3){
        cpu_quanta_per_program = 1;
    }

    if(policyNumber == 2){
        //keep running programs while ready queue is not empty
        while(readyQueuePop(0,false).PC != -1)
        {
            PCB aPCB = readyQueuePop(0,false);

            
            if(aPCB.pageTable[aPCB.pointer] == -1 && aPCB.pointer < aPCB.numOfpages) {
                int fs_index = fs_load_page(aPCB.fileName, (aPCB.pointer)*3);
                if(fs_index != -1) {
                    aPCB.pageTable[aPCB.pointer] = fs_index/3;
                } else {
                    int victim_index = evict_LRU();

                    int fs_index = fs_load_page(aPCB.fileName, (aPCB.pointer)*3);
                    aPCB.pageTable[aPCB.pointer] = fs_index/3;
                    LRU_setIndex(aPCB.pageTable[aPCB.pointer], 0);
                }
                // place at back of ready queue
                readyQueuePop(0, true);
                enqueueEnd(&aPCB);
            } else {
            
                int errorCode_loadPCB_toCPU = runningCPU(&aPCB);

                readyQueuePop(0, true);
                if(errorCode_loadPCB_toCPU == 1) {
                    enqueueEnd(&aPCB);
                }
                
            }
            
        }
        
        reset_fs();
        frame_store_LRU_init();
    }

    if(policyNumber == 0){
        
        while(readyQueuePop(0,false).PC != -1)
        {
            PCB firstPCB = readyQueuePop(0,false);
            loadPCB_toCPU(firstPCB.PC);
            
            int errorCode_loadPCB_toCPU = callCPU(cpu_quanta_per_program, firstPCB.end);
            
            if(errorCode_loadPCB_toCPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem(firstPCB.start, firstPCB.end);
                readyQueuePop(0,true);
            }
            if(errorCode_loadPCB_toCPU == 0){
                //the head PCB program has finished its quanta, it need to be put to the end of ready queue
                firstPCB.PC = getCPU_IP();
                readyQueuePop(0,true);
                enqueueEnd(&firstPCB);
            }
        }
    }

    //scheduling policy for 1: SJF
    if(policyNumber == 1){
        while(!isReadyQueueEmpty())
        {
            //task with the lowest lines of codes runs first
            int task_index_with_the_least_lines;
            int task_lines = MAX_INT;
            //get the lowest job length 
            for(int i = 0; i < READYQUEUE_SIZE; i++){
                if((*my_readyQueue[i]).start != -1 && 1 + (*my_readyQueue[i]).end - (*my_readyQueue[i]).start < task_lines){
                    task_lines = 1 + (*my_readyQueue[i]).end - (*my_readyQueue[i]).start;
                    task_index_with_the_least_lines = i;
                }
            }

            PCB current_task_PCB = (*my_readyQueue[task_index_with_the_least_lines]);
            loadPCB_toCPU(current_task_PCB.PC);
            
            int errorCode_loadPCB_toCPU = callCPU(cpu_quanta_per_program, current_task_PCB.end);
            
            //the head PCB program has been done, time to reclaim the shell mem
            clean_mem(current_task_PCB.start, current_task_PCB.end);
            //put the current PCB into invalid mode
            terminateTaskByIndex(task_index_with_the_least_lines);
        }
    }

    //scheduling policy for 3: Aging
    if(policyNumber == 3){
        int task_index_least_job_length_score;
        int task_job_length_score = MAX_INT;

        //find job with the lowest job score
        for(int i = 0; i < READYQUEUE_SIZE; i++){
            //get the lowest job length score
            if((*my_readyQueue[i]).start != -1 && (*my_readyQueue[i]).job_length_score < task_job_length_score){
                task_job_length_score = (*my_readyQueue[i]).job_length_score;
                task_index_least_job_length_score = i;
            }
        }
        //move the task with the lowest job score to the front of the queue
        PCB job_with_lowest_job_score = readyQueuePop(task_index_least_job_length_score,true);
        enqueueFront(&job_with_lowest_job_score);
        
        while(!isReadyQueueEmpty())
        {
            //task with the lowest job length score runs first
            //in this case, the task with the lowest job length score is the first task in queue
            task_job_length_score = (*my_readyQueue[0]).job_length_score;
            task_index_least_job_length_score = 0;

            PCB current_task_PCB = (*my_readyQueue[task_index_least_job_length_score]);
            loadPCB_toCPU(current_task_PCB.PC);
            
            int errorCode_loadPCB_toCPU = callCPU(cpu_quanta_per_program, current_task_PCB.end);

            if(errorCode_loadPCB_toCPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem((*my_readyQueue[task_index_least_job_length_score]).start, (*my_readyQueue[task_index_least_job_length_score]).end);
                readyQueuePop(task_index_least_job_length_score, true);
                task_job_length_score = MAX_INT;
            }

            if(errorCode_loadPCB_toCPU == 0){
                //the head PCB program has finished its quanta
                (*my_readyQueue[task_index_least_job_length_score]).PC = getCPU_IP(); // update the PC for the PCB
                //Age all the tasks (other than the current executing task) in queue by 1
                for(int i = 0; i < READYQUEUE_SIZE; i++){
                    //get the lowest job length score
                    if((*my_readyQueue[i]).start != -1 && (*my_readyQueue[i]).job_length_score > 0 && i != task_index_least_job_length_score){
                        (*my_readyQueue[i]).job_length_score -= 1;
                    }
                }
            }
            
            //if the first task job score is not the lowest, 
            //then move the first task to the end 
            //and the lowest job score task to the front
            for(int i = 0; i < READYQUEUE_SIZE; i++){
                //get the lowest job length score
                if((*my_readyQueue[i]).start != -1 && (*my_readyQueue[i]).job_length_score < task_job_length_score){
                    task_job_length_score = (*my_readyQueue[i]).job_length_score;
                    task_index_least_job_length_score = i;
                }
            }
            if(task_index_least_job_length_score != 0){
                //pop the task with the lowest job score 
                PCB lowest_job_score_task = readyQueuePop(task_index_least_job_length_score, true);
                //move the frst task to the end
                PCB first_pcb = readyQueuePop(0, true);
                enqueueEnd(&first_pcb);
                //move the lowest job score task to the front
                enqueueFront(&lowest_job_score_task);
            }
        
        }
    }

    //clean up
    clean_readyQueue();
    freeCPU();

    return errorCode;
}
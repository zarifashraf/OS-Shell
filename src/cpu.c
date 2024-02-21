#include "cpu.h"
#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"
#include "readyQueue.h"
#include "kernel.h"
#include "mmu.h"
#include "util.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


CPU myCPU = {.time_slice =0};

int getCPU_IP(){
    return myCPU.IP;
}

void setCPU_IP(int pIP){
    myCPU.IP = pIP;
}

void loadPCB_toCPU(int PC){
    setCPU_IP(PC);
}

// setting time slice equal to 2 signifies that file has reached an end
void freeCPU(){
    myCPU.time_slice = 2;
}

/*run "time_slice" lines of code, starting from cpu.IP
 *
 * returns: 2: file reaches an end, 0: no error
 */

int callCPU(int time_slice, int end){
    myCPU.time_slice = time_slice;
    int error_code;
    while (myCPU.time_slice != 0){
        
        strncpy(myCPU.IR, get_var_store_line(myCPU.IP),1000);
        parseInput(myCPU.IR);
        if(end == myCPU.IP){
            error_code = 2;
            return error_code;
        }
        myCPU.IP = myCPU.IP + 1;
        myCPU.time_slice -= 1;
    }
    
    error_code = 0;
    return error_code;
}

int runningCPU(PCB *myPCB) {

    if(myPCB->fs_index == 0) {
        
        for(int i = 0; i < 2; i++) {
            char* line = get_frame_store_line(myPCB->pageTable[myPCB->pointer]*3 + myPCB->fs_index);
            if(strcmp(line, "none") == 0) {
                return 2;
            } 
            
            else {
                parseInput(line);
                LRU_extend();
                LRU_setIndex(myPCB->pageTable[myPCB->pointer], 0);
                myPCB->fs_index+=1;
            }
        }
        
        myPCB->fs_index = 2;
        char* line = get_frame_store_line(myPCB->pageTable[myPCB->pointer]*3 + myPCB->fs_index);
        if(strcmp(line, "none") == 0) {
            
            return 2;
        } 
        
        else {

            return 1;
        }

    } 
    
    else if(myPCB->fs_index == 1) {

        for(int i = 0; i < 2; i++) {
            char* line = get_frame_store_line(myPCB->pageTable[myPCB->pointer]*3 + myPCB->fs_index);
            if(strcmp(line, "none") == 0) {
                return 2;
            } 
            
            else {
                parseInput(line);
                LRU_extend();
                LRU_setIndex(myPCB->pageTable[myPCB->pointer], 0);
                myPCB->fs_index+=1;
            }
        }

        myPCB->pointer += 1;
        myPCB->fs_index = 0;
        if(myPCB->pointer == myPCB->numOfpages) {
            return 2;
        } 
        else {
                        return 1;
        }

    } else if(myPCB->fs_index == 2) {

        char* line = get_frame_store_line(myPCB->pageTable[myPCB->pointer]*3 + myPCB->fs_index);
        parseInput(line);
        LRU_extend();
        LRU_setIndex(myPCB->pageTable[myPCB->pointer], 0);

        myPCB->pointer += 1;
        myPCB->fs_index = 0;

        if(myPCB->pointer == myPCB->numOfpages) {
        
            return 2;
        }

        if(myPCB->pageTable[myPCB->pointer] != -1) {    

            char* line = get_frame_store_line(myPCB->pageTable[myPCB->pointer]*3 + myPCB->fs_index);
            parseInput(line);
            LRU_extend();
            LRU_setIndex(myPCB->pageTable[myPCB->pointer], 0);
        }
        
         else {

            int fs_index = fs_load_page(myPCB->fileName, (myPCB->pointer)*3);
            if(fs_index == -1) {   // if not found
                int victim_index = evict_LRU();
                fs_index = fs_load_page(myPCB->fileName, (myPCB->pointer)*3);
                myPCB->pageTable[myPCB->pointer] = fs_index/3;
            }
            LRU_setIndex(myPCB->pageTable[myPCB->pointer], 0);
            return 1;
        }

        myPCB->fs_index += 1;
        line = get_frame_store_line(myPCB->pageTable[myPCB->pointer]*3 + myPCB->fs_index);
        if(strcmp(line, "none") == 0) {
            
            return 2;
        } 
        
        else {
            
            return 1;
        }

    } else {
        printf("Encountered an error.\n");
    }
}
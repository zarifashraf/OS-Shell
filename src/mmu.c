#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "mmu.h"
#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"
#include "readyQueue.h"
#include "util.h"

int fs_load_file(char* arrayOf_File[]) {
    
    int numOf_Files = 0;
    for(int i = 0; i < 3; i++) {
        if(arrayOf_File[i] != NULL) {
            numOf_Files++;
        }
    }
    
    int count[numOf_Files];
    int size[numOf_Files];
    char* fileName[3];
    
    for(int i = 0; i < 3; i++) {
        if(arrayOf_File[i] != NULL) {
            FILE *file_;
            char c;
            int lines = 0;
            file_ = fopen(arrayOf_File[i], "rt");
            
            c = getc(file_);
            while (c != EOF)
            {
                if (c == '\n') {
                    lines = lines + 1;
                }
                c = getc(file_);
            }
            fclose(file_); 

            fileName[numOf_Files-i-1] = (char*)malloc(sizeof(char) * (strlen(arrayOf_File[i]) + 1 ) );
            strcpy(fileName[numOf_Files-i-1], arrayOf_File[i]);   
            size[numOf_Files-i-1] = lines+1;
        }
    }
    for(int i = 0; i < numOf_Files; i++) { 
        count[i] = 0;
        for(int j = 0; j < 10; j++) {
            PCB* curPCB = get_readyQueueAt(j);
            if(curPCB->pid == NULL) {
                continue;
            }
            if(curPCB->fileName != NULL && strcmp(curPCB->fileName, fileName[i]) == 0) {
                int val = (size[i] / 3) + ((size[i] % 3) != 0);
                curPCB->numOfpages = val;
            }
        }
    }

    int fs_index = 0;
    int l = 0;
    int loop = 2;
    for(int a = 0; a < loop; a++) {
        l = 0;
        for(int i = 0; i < numOf_Files; i++) {
            if(count[i] < size[i]) {
                int fs_index = fs_load_page(fileName[i], count[i]);
                if(fs_index == -1) {
                    return 1;
                }
                count[i] += 3;

                for(int j = 0; j < 10; j++) {
                    PCB* curPCB = get_readyQueueAt(j);
                    if(curPCB->pid == NULL) {
                        continue;
                    }
                    if(curPCB->fileName != NULL && strcmp(curPCB->fileName, fileName[i]) == 0) {

                        curPCB->pageTable[curPCB->index_init_pt] = fs_index/3;
                        curPCB->index_init_pt = curPCB->index_init_pt+1;
                    }
                }
            } else {
                l += 1;
            }
        }
        if(l == numOf_Files) {
            break;
        }
    }
    return 0;

}


int findFrame() {
    int i = 0;
    while(i < fr_size) {    
        if(strcmp(get_frame_store_line(i), "none") == 0) {
            return i;
        }
        i = i+3;
    }
    return -1;
}

int fs_load_page(char* filename, int pageNum) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        return 1;
    }
    char line[1000];
    char buffer[1000];
    int i = 0;

    while(i < pageNum) {
        fgets(buffer, 999, file);
        i++;
    }
    int j = 0;
    int current = findFrame();
    int result = current;

    if(current != -1) {
        while(fgets(line, 999, file) && j < 3 && current < fr_size) {
            mem_set_value_fs(current, strdup(line));
            current++;
            j++;
        }
    }
    fclose(file);
    return result;

}

int evict_LRU() {
    int remove = LRU_getIndex()*3;
    clean_fs_and_print(remove, remove + 3);
    return remove;

}
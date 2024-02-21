#ifndef PCB_H
#define PCB_H

typedef struct
{
    char* pid;
    int PC;
    int start;
    int end;
    int job_length_score;
    char* fileName;
    int pageTable[100];
    int index_init_pt;
    int fs_index;
    int pointer;
    int numOfpages;
}PCB;

PCB * makePCB(int start, int end, char* pid);

#endif

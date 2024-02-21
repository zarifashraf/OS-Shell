#include "stdlib.h"
#include "stdio.h"

char *mem_get_value(char *var);
char* get_var_store_line(int line);

void mem_set_value(char *var, char *value);
void mem_set_value_fs(int index, char *value_in);

char* mem_get_value_fs(char *var_in);
char* get_frame_store_line(int line);

int addFileToMemory(FILE* fp, int* pStart, int* pEnd, char* fileID);

void clean_mem(int start, int end);
void clean_fs_and_print(int start, int end);

void frame_store_LRU_init();
void LRU_extend();

int LRU_getIndex();
void LRU_setIndex(int index, int toSet);

void reset_vs();
void reset_fs();




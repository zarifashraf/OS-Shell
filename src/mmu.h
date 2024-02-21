#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "shellmemory.h"
#include "pcb.h"
#include "kernel.h"
#include "util.h"

int evict_LRU();
int findFrame();

int fs_load_file(char* fileArr[]);
int fs_load_page(char* filename, int pageNum);

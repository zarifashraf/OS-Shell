#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

#include "util.h"

struct memory_struct{
	char *var;
	char *value;
};

struct memory_struct frame_store[fr_size]; 
int frame_storeLRU[fr_size/3];

struct memory_struct var_store[var_size];

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<var_size; i++){
		if (strcmp(var_store[i].var, var_in) == 0){
			var_store[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<var_size; i++){
		if (strcmp(var_store[i].var, "none") == 0){
			var_store[i].var = strdup(var_in);
			var_store[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

// Set key value pair
void mem_set_value_fs(int index, char *value_in) {
	frame_store[index].value = strdup(value_in);
}

//get value based on input key - variable store
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<var_size; i++){
		if (strcmp(var_store[i].var, var_in) == 0){
			return strdup(var_store[i].value);
		} 
	}
	return "Variable does not exist";

}

char* get_var_store_line(int line){
	return var_store[line].value;
}

char* get_frame_store_line(int line){
	return frame_store[line].value;
}

void clean_mem(int start, int end){
    
	for(int i = start; i <= end; i ++){
        var_store[i].var = "none";
		var_store[i].value = "none";
    }
}

void clean_fs_and_print(int start, int end){
	
	printf("%s\n", "Page fault! Victim page contents:\n");
    
	for(int i = start; i < end; i ++){
		if(strcmp(frame_store[i].value, "none") != 0) {
			if(frame_store[i].value[strlen(frame_store[i].value)-1]!='\n') {
				printf("%s\n", frame_store[i].value);
			} 
			
			else {
				printf("%s", frame_store[i].value);
			}
		}
        
		frame_store[i].var = "none";
		frame_store[i].value = "none";
    }

	printf("\n");
	printf("%s\n", "End of victim page contents.");
}

void reset_fs(){
	
	for(int a = 0; a < fr_size; a++) {
		
		frame_store[a].var = "none";
		frame_store[a].value = "none";
	}

}

void reset_vs(){

	for (int a = 0; a < var_size; a++){		
		var_store[a].var = "none";
		var_store[a].value = "none";
	
	}

}

void frame_store_LRU_init() {
	for(int i = 0; i < fr_size/3; i++) {
		frame_storeLRU[i] = 0;
	}
	
}

void LRU_extend() {
	for(int i = 0; i < fr_size/3; i++) {
		if(strcmp(frame_store[i*3].value, "none") != 0) {
			frame_storeLRU[i] += 1;
		}
	}
}

int LRU_getIndex() {
	int most = 0;
	int result = 0;
	
	for(int i = 0; i < fr_size/3; i++) {
		if(frame_storeLRU[i] >= most) {
			most = frame_storeLRU[i];
			result = i;
		}
	}
	return result;
}

void LRU_setIndex(int index, int toSet) {
	frame_storeLRU[index] = toSet;
}

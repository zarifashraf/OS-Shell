#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>


#include "shellmemory.h"
#include "shell.h"
#include "mmu.h"
#include "kernel.h"
#include "readyQueue.h"
#include "util.h"

// MAX_ARGS_SIZE increased to 7 to fit at least 5 values for 'set'
int MAX_ARGS_SIZE = 7;

int help();
int quit();
int badcommand();
int set(char* var, char* value);
int print(char* var);
int run(char* script);

// Added in A1
int badcommandFileDoesNotExist();
int badcommandTooManyTokens();
int echo(char* string);
int my_ls();
int my_mkdir(char *dirname);
int my_touch(char *filename);
int badcommandMyMkDir();
int my_cd(char *dirname);
int badcommandMyCD();

// Added in A2
int exec(char *file1, char *file2, char *file3, char* policy);
int badCommandIncorrectSchedulingPolicy();
int badcommandOutOfMem();
int badCommandReadyQueueFull();
int badcommandDupFileName();
int errorCodeRedirect(int errorCode);

// Added in A1 to remove new files and directories created
void removeFiles();
void removeDirectories();
void returnToBase();

// Array for deleting files created by my_touch
char **filenames = NULL;
int num_filenames = 0;

// Array for deleting directories created by my_touch
char **dirnames = NULL;
int num_dirnames = 0;

// Remember current directory using help() function which is always called at first
char base_wd[108];

int interpreter(char* command_args[], int args_size){

	int i;

	// args_size > MAX_ARGS_SIZE removed to output correct error
	if ( args_size < 1) {
		return badcommand();
	}

	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} 
	
	else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} 
	
	else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3) return badcommand();
		if (args_size > MAX_ARGS_SIZE) return badcommandTooManyTokens();

		char* value = (char*)calloc(1,150);
		char spaceChar = ' ';

		for(int i = 2; i < args_size; i++){
			strncat(value, command_args[i], 30);
			if(i < args_size-1){
				strncat(value, &spaceChar, 1);
			}
		}
		return set(command_args[1], value);
	
	} 
	
	else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} 
	
	else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} 

	else if (strcmp(command_args[0], "echo")==0) {
		if (args_size != 2) return badcommand();
		return echo(command_args[1]);
	
	} 
	
	else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size != 1) return badcommand();
		return my_ls();
	}

	else if (strcmp(command_args[0], "my_mkdir")==0) {
		if (args_size !=2) return badcommandMyMkDir();
		return my_mkdir(command_args[1]);
	}

	else if (strcmp(command_args[0], "my_touch")==0) {
		if (args_size != 2) return badcommand();
		return my_touch(command_args[1]);
	}

	else if (strcmp(command_args[0], "my_cd")==0) {
		if (args_size != 2) return badcommandMyCD();
		return my_cd(command_args[1]);
	}
	
	else if (strcmp(command_args[0], "exec")==0) {
		
		if (args_size <= 2 || args_size >5) {
			return badcommand();
		}
		if(args_size == 3){
			return exec(command_args[1],NULL,NULL,command_args[2]);
		}
		else if(args_size == 4){
			return exec(command_args[1],command_args[2],NULL,command_args[3]);
		}
		else if(args_size == 5){
			return exec(command_args[1],command_args[2],command_args[3],command_args[4]);
		}
	}
	else if (args_size > 7) {
		return badcommandTooManyTokens();
	}

    else {
		return badcommand();
	} 
}

int help(){

	// Remember the base directory
	getcwd(base_wd, sizeof(base_wd));
	
	// Continue with actual implementation

	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT";

	printf("%s\n", help_string);
	return 0;

}

int quit(){

	// Going back to base step by step and erasing newly created files and directories
	returnToBase();
	removeDirectories();
	removeFiles();

	free(filenames);
	free(dirnames);


	deleteBS();
	printf("%s", "Bye!");
	exit(0);
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

int badcommandFileDoesNotExist(){
	printf("%s\n", "Bad command: File not found");
	return 1;
}

int set(char* var, char* value){
	mem_set_value(var, value);
	return 0;
}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){

    int result = 0;
	char* errCode = "";
	char file_1[100] = "";

	errCode = kernelInit(script);
	
    if(strcmp(errCode, "11") == 0){
		result = strtol(errCode, NULL, 10);
		return errorCodeRedirect(result);
	} 
    
    else {
		strcpy(file_1, errCode);
	}

    char* file[] = {file_1, NULL, NULL};

	reset_fs();
	fs_load_file(file);
	
    scheduler(2);
    
	result = strtol(errCode, NULL, 10);
	
    return result;
}

int echo(char* string){ 
	

	if(strncmp(string, "$", 1) == 0){
		string++;	//string points to the next address 
		
		if (strcmp(mem_get_value(string), "Variable does not exist") == 0) { 
			printf("\n");
		}

		else {
			print(string);
		}
	}
	else{
		printf("%s\n", string); 
	}

	return 0;
}

int my_ls(){
	int errCode = system("ls -v | LC_ALL=C sort -f");
	return errCode;
}

int my_mkdir(char *dirname) {
  	
	char command[108];
	//avoid unprintable ASCII
	command [0] = '\0';
	int wordCount = 0;
  	
	if (dirname[0] == '$') {
    	dirname++;	//dirname begins from the next character
    	char *value = mem_get_value(dirname);	// get the value associated with the name
   	 	
		for (int i = 0; (i < strlen(value)); i++) {
        	
			if (value[i] == ' '	|| value[i] == '\0') {
            	wordCount++;
        	}
    	}
		
		if ((strcmp(value, "Variable does not exist") == 0) || value == NULL || wordCount > 1) {
      		return badcommandMyMkDir();
    	}

		int len = strlen(value);
  		int j = 0;
  		char new_str[len + 1];

  		for (int i = 0; i < len; i++) {
    		if (isalnum(value[i])) {
      			new_str[j++] = value[i];
    		}
  		}

  		new_str[j] = '\0';
  		strcpy(value, new_str);		

		strcpy(command, "mkdir ");
  		strcat(command, value);
		
		// Replacing dirname with value for appending to dirnames list
		strcpy(dirname, value);
	} 
	
	else {
		strcpy(command, "mkdir ");
  		strcat(command, dirname);
  	} 

	int errorCode = system(command);

	if (errorCode != 0) {
    	return badcommandMyMkDir();
  	}

	// TRYING TO APPEND DIRNAMES TO THE LIST
	num_dirnames++;
	dirnames = realloc(dirnames, num_dirnames * sizeof(char *));
	dirnames[num_dirnames - 1] = malloc(100 * sizeof(char));

	sprintf(dirnames[num_dirnames - 1], "%s", dirname);

  	// EFFORT ENDS HERE
	return errorCode;
}


int badcommandMyMkDir(){
	printf("%s\n", "Bad command: my_mkdir");
	return 4;
}

int my_touch(char *filename) {
	
	int i, length = strlen(filename);
  	
	for (i = 0; i < length; i++) {
    	if (!isalnum(filename[i])) {
      		return badcommand();
    	}
  	}
  
  	char command[107];
	command [0] = '\0';
  	strcpy(command, "touch ");
  	strcat(command, filename);
  	int errCode = system(command);

  	// TRYING TO APPEND FILENAME TO THE LIST
  	if (errCode == 0) {
		num_filenames++;
		filenames = realloc(filenames, num_filenames * sizeof(char *));
		filenames[num_filenames - 1] = malloc(100 * sizeof(char));

		sprintf(filenames[num_filenames - 1], "%s", filename);
  	}
  	// EFFORT ENDS HERE

  	return errCode;
}

int my_cd(char *dirname) {

	char command[108];
	int errCode = chdir(dirname);
  
  	if (errCode != 0) {
    	return badcommandMyCD();
  	}

  	return errCode;	
}

int badcommandMyCD(){
	printf("%s\n", "Bad command: my_cd");
	return 5;
}

void removeFiles() {

	char command[108];

	for (int i = 0; i < num_filenames; i++) {
		memset(command, 0, sizeof(command));
		sprintf(command, "rm -r %s >/dev/null 2>&1", filenames[i]);
		system(command);
	}
}

void removeDirectories() {

	char command[108];

	for (int i = 0; i < num_dirnames; i++) {
		memset(command, 0, sizeof(command));
		sprintf(command, "rm -r %s >/dev/null 2>&1", dirnames[i]);
		system(command);
	}
}

void returnToBase() {
	chdir(base_wd);
	// return getcwd(cwd, sizeof(cwd));
}

int badCommandIncorrectSchedulingPolicy(){
	printf("%s\n", "Bad command: incorrect scheduling policy");
	return 1;
}

int badcommandOutOfMem(){
	printf("%s\n", "Bad command: out of shell memory");
	return 1;
}

int badCommandReadyQueueFull(){
	printf("%s\n", "Bad command: ready queue full");
	return 1;
}

int badcommandDupFileName(){
	printf("%s\n", "Bad command: same file name");
	return 1;
}

int exec(char *file1, char *file2, char *file3, char* policy){

    
    int policyNumber = getSchedulingPolicyNo(policy);
	
    if(policyNumber == 15){
		return errorCodeRedirect(policyNumber);
	}

    char* errorCode = "";
	char file_1[100] = "";
	char file_2[100] = "";
	char file_3[100] = "";
	
    if(file1 != NULL){
        errorCode = kernelInit(file1);

		if(strcmp(errorCode, "11") == 0){
			int result = 0;
			result = strtol(errorCode, NULL, 10);
			return errorCodeRedirect(result);
		} else {
			strcpy(file_1, errorCode);
		}
    }
    if(file2 != NULL){
        errorCode = kernelInit(file2);

		if(strcmp(errorCode, "11") == 0){
			int result = 0;
			result = strtol(errorCode, NULL, 10);
			return errorCodeRedirect(result);
		} else {
			strcpy(file_2, errorCode);
		}
    }
    if(file3 != NULL){
        errorCode = kernelInit(file3);
		if(strcmp(errorCode, "11") == 0){
			int result = 0;
			result = strtol(errorCode, NULL, 10);
			return errorCodeRedirect(result);
		}
		else {
			strcpy(file_3, errorCode);
		}
    }

	char* arr[] = {file_1, file_2, file_3};

	reset_fs();
	
    fs_load_file(arr);
    scheduler(policyNumber);
	
    int result = 0;

	return result;
}

int errorCodeRedirect(int errorCode){

	if(errorCode == 11){
		return badcommandFileDoesNotExist();
	}
	else if (errorCode == 21)
	{
		return badcommandOutOfMem();
	}
	else if (errorCode == 14)
	{
		return badCommandReadyQueueFull();
	}
	else if (errorCode == 15){
		return badCommandIncorrectSchedulingPolicy();
	}
	else{
		return 0;
	}
}

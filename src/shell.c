#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "interpreter.h"
#include "shellmemory.h"
#include "kernel.h"
#include "readyQueue.h"
#include "util.h"

int MAX_USER_INPUT = 1000;
int parseInput(char ui[]);

int main(int argc, char *argv[]) {

	createBS();

	printf("%s\n", "Shell version 1.2 Created January 2023\n");
	printf("Frame Store Size = %d; Variable Store Size = %d\n", fr_size, var_size);
	

	char prompt = '$';  				// Shell prompt
	char userInput[MAX_USER_INPUT];		// user's input stored here
	int errorCode = 0;					// zero means no error, default

	//init user input
	for (int i=0; i<MAX_USER_INPUT; i++)
		userInput[i] = '\0';
	
	//init shell memory
	reset_vs();
	reset_fs();
	
	frame_store_LRU_init();
	readyQueue_init();

	//initialize random number generator, for fileID and pid generation
	srand(time(NULL));
	
	while(1) {						
		
		if (isatty(fileno(stdin))) {
			printf("%c ",prompt);
		}

		fgets(userInput, MAX_USER_INPUT-1, stdin);
        
		if (feof(stdin)){
            freopen("/dev/tty", "r", stdin);
        }

		if(strlen(userInput) > 0) {
            errorCode = parseInput(userInput);
            if (errorCode == -1) {
				exit(99);
			}	// ignore all other errors
            memset(userInput, 0, sizeof(userInput));
		}
	}

	return 0;

}

int parseInput(char ui[]) {
	char tmp[200];
	char *words[100];							
	int a = 0;
	int b;							
	int w=0; // wordID	
	int errorCode;
	
	// skip white spaces AND tabs
	for (a=0; (ui[a]==' ' || ui[a] == '\t') && a < MAX_USER_INPUT; a++) {
		continue;	// do nothing, only increment the count for a
	};

	while(ui[a] != '\n' && ui[a] != '\0' && a < MAX_USER_INPUT) {
		
		for(b=0; ui[a] !=';' && ui[a] !='\0' && ui[a] !='\n' && ui[a] !=' ' && a< MAX_USER_INPUT; a++, b++) {
			tmp[b] = ui[a];	 // extract a word
		}

		tmp[b] = '\0';	// signifies end of the inidivdual word or command

		words[w] = strdup(tmp);

		if(ui[a]==';'){
			w++;

			errorCode = interpreter(words, w);
			
			if(errorCode == -1){
				return errorCode;
			}

			a++;
			w = 0;

			for(; (ui[a]==' ' || ui[a] == '\t') && a <MAX_USER_INPUT; a++);	{	// skip white spaces and tabs
				continue; 
			}

		}
		
		w++;
        if(ui[a] == '\0'){
            break;
        }
        a++; 
	}
	errorCode = interpreter(words, w);

	return errorCode;
}
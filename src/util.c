#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>
#include<unistd.h>

int number = 1;

int resetIndex() {
    number = 1; 
    return 0;
}

void createBS() {
	
	if(0 != access("BackingStore", F_OK)) {
		system("mkdir ./BackingStore");
	}
	else {
		system("rm -rf ./BackingStore/*");
	}
}

void deleteBS() {

	int error_code = 0;
	char deleteBackingStore[100];

	strcpy(deleteBackingStore, "rm -r ./BackingStore");
	error_code = system(deleteBackingStore);

}

void clearBS() {
	int error_code = 0;
	char clearBackingStore[100];

	strcpy(clearBackingStore, "rm -rf ./BackingStore/*");
	error_code = system(clearBackingStore);
}

char* namingFile(int indice) {
    char* filename = malloc(sizeof(char) * 50);
    strcpy(filename, "BackingStore/file");   //copying "file" into string filename

    char number[100];
    sprintf(number, "%d", indice);   //converting int to char
    
    strcat(filename, number);   //concatenating two char
    return filename;
}

char* loadScript(char* file) {
    //create a file in the BackingStore
    char* name = namingFile(number);

    FILE *fp1, *fp2;
    char c;
    fp1 = fopen(file, "r");
    
    if (fp1 == NULL)
    {
        printf("Cannot open file %s \n", file);
        return "11";
    }
  
    // Open another file for writing
    fp2 = fopen(name, "w");
    if (fp2 == NULL)
    {
        printf("Cannot open file %s \n", name);
        return "11";
    }
  
    // Read contents from file
    c = fgetc(fp1);
    while (c != EOF)
    {
        fputc(c, fp2);
        c = fgetc(fp1);
    }
    fclose(fp1);
    fclose(fp2);

    number += 1;

    return name;
}


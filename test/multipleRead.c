#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//http://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
//http://stackoverflow.com/questions/3889992/how-does-strtok-split-the-string-into-tokens-in-c

int main(int argc, char *argv[])
{
	//initial configuration
	char fileInput[129][256];
	int counter = 0;

	//opening file
	FILE *fp = fopen(argv[1],"r"); // read mode
 
	if( fp == NULL ){
    	printf("Error while opening the file.\n");
    }

	while (fgets (fileInput[counter], 256, fp)) {
    	counter++;
	}
	
	if (ferror(fp)) {
    	fprintf(stderr,"Oops, error reading the file\n");
	}	
	
    fclose(fp);

    int i;
    int found;
    char *token;
    char number[256] = "nothng";
    char process[256] = "nothing";

    for(i=0;i < counter; i++){
    	token = strtok(fileInput[i], " ");
    	strcpy(process,token);
    	printf( "%s", process );
    	token = strtok(NULL, " ");
    	strcpy(number,token);
    	printf( " %s", number );
    }
    printf("\n");
}
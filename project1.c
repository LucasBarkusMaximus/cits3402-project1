#include <omp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIA 0.000001
#define COL 4400
#define KEYCHARS 70000
#define DATACHARS 45000

//defined outside of functions to avoid warnings, bad practice
float colArray[COL];
int keyArray[COL];

float * input_data(FILE * fp){

	char str[DATACHARS];
	fp = fopen("data.txt", "r");
	if(fgets(str, DATACHARS, fp)!=NULL){
		//testing
		//printf("%s\n", str);
		const char s[2] = ",";
   		char *token;
		token = strtok(str, s);
		int i = 0;
   		while( token != NULL ) 
  	 	{
  	 	//testing
  		//printf("%s\n", token);
    	colArray[i] = atof(token);
     	i++;
    	token = strtok(NULL, s);
   		}
	}
	fclose(fp);
	//testing
	int a  = sizeof(colArray) / sizeof(float);
	printf("%d\n", a);
	return colArray;
}

int * input_keys(FILE * fp){
	char str[KEYCHARS];
	fp = fopen("keys.txt", "r");

	if(fgets(str, KEYCHARS, fp)!=NULL){
		//testing
		//printf("%s\n", str);
		const char s[2] = " ";
   		char *token;
		token = strtok(str, s);
		int i = 0;
   		while( token != NULL ){
   			//testing
  	 		//printf("%s\n", token);
    		keyArray[i] = atoi(token);
    		i++;
    		token = strtok(NULL, s);
    	}
	}
	fclose(fp);
	//testing
	int a  = sizeof(keyArray) / sizeof(int);
	printf("%d\n", a);
	return keyArray;
}

/*
sort single column
generate neighborhoods & blocks
move onto next column
generate neighborhoods & blocks
append blocks to blocks list, sorting as you go
check collisions, store
etc etc
*/

// I/O read in data (columns)

// match columns with keys

// sort ascending values

// generate neighbourhood

// find & make blocks

// determine signature

// create signature matrix (to store IDs in)

// store signature & rowID

// determine collisions

// make collision table


int main() {
	clock_t begin = clock();
	FILE *f;
	FILE *g;
	input_data(f);
	input_keys(g);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	return 0;
}
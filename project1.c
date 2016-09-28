
#include <omp.h>
#include <string.h>
//#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define DIA 0.000001
#define COL 4400
#define KEYCHARS 70000
#define DATACHARS 45000


//defined outside of functions to avoid warnings, bad practice
double colArray[COL];
double keyArray[COL];

double * input_data(FILE * fp){

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
  		printf("%s\n", token);
    	colArray[i] = atof(token);
     	i++;
    	token = strtok(NULL, s);
   		}
	}
	fclose(fp);
	//testing
	//int a  = sizeof(colArray) / sizeof(float);
	//printf("%d\n", a);
	return colArray;
}

double * input_key(FILE * fp){
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
  	 		printf("%s\n", token);
    		keyArray[i] = (double) atoi(token);
    		i++;
    		token = strtok(NULL, s);
    	}
	}
	fclose(fp);
	//testing
	//int a  = sizeof(keyArray) / sizeof(int);
	//printf("%d\n", a);
	return keyArray;
}



			


int main() {
	clock_t begin = clock();

	//inputs from text files
	FILE *f;
	FILE *g;
	double *colArr;
	double *keyArr;
	//an array for values and one for keys
	colArr = input_data(f);
	keyArr = input_key(g);

	//TODO: transfer values from arrays to matrix to be sorted

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("%d seconds", time_spent);

	return 0;
}

//gcc -fopenmp -o project1 project1.c -std=c99

// generate neighbourhood

// find & make blocks

// determine signature

// create signature matrix (to store IDs in)

// store signature & rowID

// determine collisions

// make collision table
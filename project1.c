
#include <sys/time.h>
#include <omp.h>
#include <windows.h>
#include <string.h>
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
	//printf("%d\n", a);7
	return keyArray;
}

int compare(const void *a, const void *b) {
   double x1 = *(const double*)a;
   double x2 = *(const double*)b;
   if (x1 > x2) return  1;
   if (x1 < x2) return -1;
   return 0;
}	

generate_neighborhood(){
	//neighborhood[Suburb][street]
	int neighbourhood[1000][100];
	neighbourhood[0][0] = colArray[0];
	for(int i = 0;i<COL; i++)
		int j = 1;
		while(Math.abs(colArray[i]-colArray[i+j])<DIA){
			neighbourhood[i][j] = colArray[i+j]
			j++
		}
}
			


int main() {
 	struct timeval start, end;
 	gettimeofday(&start, NULL);

	//inputs from text files
	FILE *f;
	FILE *g;
	double *colArr;
	double *keyArr;
	//an array for values and one for keys
	colArr = input_data(f);
	keyArr = input_key(g);

	//TODO: transfer values from arrays to matrix to be sorted
	double col_key[4][2] = {{0.047039, 12135267736472}, {0.037743, 99115488405427}, 
		{0.051712, 30408863181157}, {0.034644, 27151991364761}};
   
	printf("OG: \n");
    for(int i = 0; i < 4; i++) {
    	printf("(%f, %f) \n", col_key[i][0], col_key[i][1]);
    }

    qsort(col_key, 4, sizeof(*col_key), compare);

    printf("Sorted: \n");
    for (int i = 0; i < 4; i++) {
    	printf("(%f, %f) \n", col_key[i][0], col_key[i][1]);
    }
	gettimeofday(&end, NULL);
  	double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
  		 end.tv_usec - start.tv_usec) / 1.e6;

  	printf("time = %12.10f seconds\n",delta);

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
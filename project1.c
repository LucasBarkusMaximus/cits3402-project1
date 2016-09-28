
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
double colArray[COL][2];


void input_data(FILE * fp,double colArray[COL][2]){

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
    	colArray[i][0] = atof(token);
     	i++;
    	token = strtok(NULL, s);
   		}
	}
	fclose(fp);
	//testing
	//int a  = sizeof(colArray) / sizeof(float);
	//printf("%d\n", a);
}

void input_key(FILE * fp,double colArray[COL][2]){
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
    		colArray[i][1] = (double) atoi(token);
    		i++;
    		token = strtok(NULL, s);
    	}
	}
	fclose(fp);
	//testing
	//int a  = sizeof(keyArray) / sizeof(int);
	//printf("%d\n", a);7
}

int compare(const void *a, const void *b) {
   double x1 = *(const double*)a;
   double x2 = *(const double*)b;
   if (x1 > x2) return  1;
   if (x1 < x2) return -1;
   return 0;
}	


/*
generate_neighborhood(){
=======
/*generate_neighborhood(){
>>>>>>> bdff6f3a2ea0c6f85f66bdff72d7295ffd11cfd6
	//neighborhood[Suburb][street]
	int neighbourhood[1000][100];
	neighbourhood[0][0] = colArray[0];
	for(int i = 0;i<COL; i++)
		int j = 1;
		while(Math.abs(colArray[i]-colArray[i+j])<DIA){
			neighbourhood[i][j] = colArray[i+j]
			j++
		}
<<<<<<< HEAD
}
*/		



//place your john hancock here pls sir
//change array[4][2] to array [COL][2]			
double signature(double array[4][2]) {
	double sig;
	//4 is arbitrary, change to array size
	for(int i = 0; i < 4; i++) {
		sig += array[i][1];
	}
>>>>>>> bdff6f3a2ea0c6f85f66bdff72d7295ffd11cfd6

    printf("%f\n", sig);
	return sig;
}

int main() {
 	struct timeval start, end;
 	gettimeofday(&start, NULL);

 	
	//inputs from text files
	FILE *f;
	FILE *g;
	double** colArr;
	//an array for values and one for keys
	input_data(f,colArray);
	input_key(g,colArray);

	//TODO: transfer values from arrays to matrix to be sorted
	
	//double col_key[4][2] = {{0.047039, 12135267736472}, {0.037743, 99115488405427}, 
	//	{0.051712, 30408863181157}, {0.034644, 27151991364761}};
   /*
	printf("OG: \n");
    for(int i = 0; i < 4; i++) {
    	printf("(%f, %f) \n", colArr[i][0], colArr[i][1]);
    }

    qsort(colArr, 4, sizeof(*colArr), compare);

    printf("Sorted: \n");
    for (int i = 0; i < 4; i++) {
    	printf("(%f, %f) \n", colArr[i][0], colArr[i][1]);
    }

    */

    signature(col_key);


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
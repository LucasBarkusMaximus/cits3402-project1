
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
float colArray[COL];
char keyArray[COL][15];

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
  		printf("%s\n", token);
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

void input_keys(FILE * fp){
	char str[KEYCHARS];
	fp = fopen("keys.txt", "r");

	if(fgets(str, KEYCHARS, fp)!=NULL){
		//testing
		printf("%s\n", str);
		const char s[2] = " ";
   		char *token;
		token = strtok(str, s);
		int i = 0;
   		while( token != NULL ){
   			//testing
  	 		printf("%s\n", token);
    		strcpy(keyArray[i], token);
    		i++;
    		token = strtok(NULL, s);
    	}
	}
	fclose(fp);
	//testing
	int a  = sizeof(keyArray) / sizeof(int);
	printf("%d\n", a);
}



typedef int (*compfn)(const void*, const void*);

struct animal { float  number;
                char name[15];
              };

struct animal array[COL];

void printarray(void);
int  compare(struct animal *, struct animal *);

int compare(struct animal *elem1, struct animal *elem2)
{
   if ( elem1->number < elem2->number)
      return -1;

   else if (elem1->number > elem2->number)
      return 1;

   else
      return 0;
}

void printarray(void)
{
   int i;

   for (i = 0; i < 10; i++)
      printf("%d:  Number %f is a %s\n",
               i+1, array[i].number, array[i].name);
}
				


int main() {
	clock_t begin = clock();
	FILE *f;
	FILE *g;
	input_keys(g);
	float *colArr;
	unsigned long long int *keyArr;
	colArr = input_data(f);

	for (int i = 0; i < COL; ++i)
	{
		array[i].number = colArr[i];
		strcpy(array[i].name,keyArray[i]);
	}
 printf("List before sorting:\n");
   printarray();

   qsort((void *) &array,              // Beginning address of array
   10,                                 // Number of elements in array
   sizeof(struct animal),              // Size of eah element
   (compfn)compare );                  // Pointer to compare function

   printf("\nList after sorting:\n");
   printarray();

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
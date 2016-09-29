
#include <sys/time.h>
#include <omp.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DIA 0.000001
#define COL 4400
#define KEYCHARS 70000
#define DATACHARS 45000
#define BLOCKSIZE 4




void input_data(FILE * fp,float arr[COL][2], int colNumber){
	int j = 0;
	char str[DATACHARS];
	fp = fopen("data.txt", "r");
	while(fgets(str, DATACHARS, fp)!=NULL){
		//testing
		//printf("%s\n", str);
		const char s[2] = ",";
   		char *token;
		token = strtok(str, s);
		int i = 0;
		
   		while( token != NULL ) 
  	 	{
  	 	//testing
  	 	if(i == colNumber){
    	arr[j][0] = atof(token);
    	arr[j][1] = (float)j;
    	//printf("float:%lf %f\n", arr[j][0], arr[j][1]);
    	j++;
    	break;
    	}else{
    	i++;
    	}
    	token = strtok(NULL, s);
   		}
	}
	fclose(fp);
	//testing
	//int a  = sizeof(colArray) / sizeof(float);
	//printf("%d\n", a);
}

void input_key(FILE * fp,double arr[COL]){
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
    		arr[i] = atof(token);
    		//sscanf(token, "%lf", colArray[i][1]);
    		//printf("float:%lf\n", arr[i][1]);
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
   float x1 = *(const float*)a;
   float x2 = *(const float*)b;
   if (x1 > x2) return  1;
   if (x1 < x2) return -1;
   return 0;
}	


double findKey(int loc, double kyArr[COL]){
return kyArr[loc];
}


void generate_neighborhood(size_t suburb, size_t street,float cArr[COL][2],double nArr[suburb][street],double kyArr[COL]){
	int sFlag = 0;
	int eFlag = 1;
	int neighbourhood = 0;
	//for each element in the column
	for(int i = 0;i<(COL-BLOCKSIZE); i++){
		//start flag is the first elemenet of a potential new neighborhood
		sFlag = i;
		//if there is a block sized overlap between the last neighborhood and this new one, shift the start of the new neighborhood up so that there is no overlap
		if(eFlag-sFlag>=BLOCKSIZE){
			i = eFlag;
		}
		//this element (i) starts a neighbourhood
		nArr[neighbourhood][0] = findKey(cArr[i][1],kyArr);
		//element to be checked for entry into current neighborhood. (the next element after i)
		int j = i+1;
		float dist  = (cArr[j][0]-cArr[i][0]);

			//printf("%d:%+8.8f\n",j, cArr[j][0]);
			//printf("%d:%+8.8f\n",i, cArr[i][0]);
			//printf("dist:%+8.8f\n", dist);
			//printf("%s\n", dist<=DIA ? "true" : "false");

		//if the new element is within dist, add it to the hood and check the next one etc.
		while(dist<=DIA){
			double key = findKey(cArr[j][1], kyArr);
			printf("row: %d Val:%f \n",j,cArr[j][0]);
			nArr[neighbourhood][j-i] = key;
			j++;
			if(j-i< street){
			dist  = (cArr[j][0]-cArr[i][0]);
			//printf("dist:%+8.8f(l)\n", dist);
			}else{
				break;
			}
		}
		//set end flag as the element after the last element in the old hood
		eFlag = j;
		//if this hood is too small to contain blocks, recycle its index in the array
		if(!nArr[start][BLOCKSIZE-1] == 0){
			start++;
			printf("good neighbourhood\n");
		}
		
	}

}
	



//place your john hancock here pls sir
//change array[4][2] to array [COL][2]
		
double signature(double array[4][2]) {
	double sig;
	//4 is arbitrary, change to array size
	for(int i = 0; i < 4; i++) {
		sig += array[i][1];
	}


    printf("%f\n", sig);
	return sig;
}

int main() {
 	struct timeval start, end;
 	gettimeofday(&start, NULL);

 	
	//inputs from text files
	FILE *f;
	FILE *g;
	//array for storing a hood
	size_t neighbourhoodNumber = 1000;
	size_t neighbourhoodSize = 100;
	float colArray[COL][2];	//an array for values and one for keys
	double neighbArray[neighbourhoodNumber][neighbourhoodSize];
	//array for storing keys
	double keyArray[COL];
	//get the first column
	input_data(f,colArray,0)
	//get the keys
	input_key(g,keyArray);
	
	printf("OG: \n");
    for(int i = 0; i < COL; i++) {
    	printf("(%f, %f) \n", colArray[i][0], colArray[i][1]);
    }

    //sort the column
    qsort(colArray, COL, sizeof(*colArray), compare);
   
    printf("Sorted: \n");
    for (int i = 0; i < COL; i++) {
    	printf("(%f, %f) \n", colArray[i][0], colArray[i][1]);
    }
    //generate all hoods for this column
     generate_neighborhood(1000,100, colArray, neighbArray, keyArray);

    //signature(col_key);


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
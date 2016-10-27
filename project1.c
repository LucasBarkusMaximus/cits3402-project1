#include <sys/time.h>
#include <omp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//number of rows in a block
#define BLOCKSIZE 4
//size of array holding each columns blocks
#define BLOCKARRAYSIZE 5000
//column length in data.txt
#define COL 4400
//size of matrix holding collisions
#define COLLISIONARRAYSIZE 2000
//size of bytes in data file
#define DATACHARS 40000//7000
//diff between values in a neighbourhood
#define DIA 0.000001
//size of bytes in key file
#define KEYCHARS 71000
//max number of neighbourhoods in a given column
#define NEIGHBOURHOODNUMBER 1000
//max size of neighbourhood in given column
#define NEIGHBOURHOODSIZE 25
//optimal number of threads - to be commented out if used on a computer with more threads
#define NUM_THREADS 4
//amount of columns read into program to compute collisions
//NB. max is 499
//NB. row 499 seems impossible to read in, even by itself
//NB. 90 was the max we could get to work on our machines so program would execute and didn't throw the time value to a number with an error in it
#define ROW 90


//read comma sperated values from text file and store the [colNumber]'th number in each line an array
void input_data(float arr[COL][2], int colNumber){
  	//open and read file
  	FILE *f;
	int j = 0;
	char str[DATACHARS];
	f = fopen("data.txt", "r");
  	//while reading a line
	while(fgets(str, DATACHARS, f)!=NULL){
		//seperate on commas
		const char s[2] = ",";
   		char *token;
		token = strtok(str, s);\
		int i = 0;
		//while not at the end of the line yet, add to array
   		while( token != NULL ) {
  	 		//store value at position
  	 		if(i == colNumber){
      		arr[j][0] = atof(token);
      		arr[j][1] = (float)j;
      		j++;
    		break;
    		} else {
    			i++;
    		}
    	 	token = strtok(NULL, s);
   		}
	}
	fclose(f);	
}

//read keys from file. Keys are seperateed by whitespace and are sequential
void input_key(double arr[COL]){
  	//open and read
  	FILE *f;
	char str[KEYCHARS];
	f = fopen("keys.txt", "r");
  	//while there are lines to read
	if(fgets(str, KEYCHARS, f)!=NULL){
	 	//split on space
		const char s[2] = " ";
   		char *token;
		token = strtok(str, s);
		int i = 0;
   		while( token != NULL ){
   		
    		arr[i] = atof(token);
    		
    		i++;
    		token = strtok(NULL, s);
    	}
	}
	fclose(f);
}

  /*
  **The following functions, twiddle()and inittwidle() were sourced from http://www.netlib.no/netlib/toms/382
  **
  **Coded by Matthew Belmonte <mkb4@Cornell.edu>, 23 March 1996.  This
  **implementation Copyright (c) 1996 by Matthew Belmonte.  Permission for use and
  **distribution is hereby granted, subject to the restrictions that this
  **copyright notice and reference list be included in its entirety, and that any
  **and all changes made to the program be clearly noted in the program text.
  **
  **Phillip J Chase, `Algorithm 382: Combinations of M out of N Objects [G6]',
  **Communications of the Association for Computing Machinery 13:6:368 (1970).
  */

//Generates combinations of elements within an array by altering the last combination produced, must be seeded with an initial combination 
//Returns one when all combinations have been produced
int twiddle(x, y, z, p)
int *x, *y, *z, *p;
  {
  register int i, j, k;
  j = 1;
  while(p[j] <= 0)
    j++;
  if(p[j-1] == 0)
    {
    for(i = j-1; i != 1; i--)
      p[i] = -1;
    p[j] = 0;
    *x = *z = 0;
    p[1] = 1;
    *y = j-1;
    }
  else
    {
    if(j > 1)
      p[j-1] = 0;
    do
      j++;
    while(p[j] > 0);
    k = j-1;
    i = j;
    while(p[i] == 0)
      p[i++] = -1;
    if(p[i] == -1)
      {
      p[i] = p[k];
      *z = p[k]-1;
      *x = i-1;
      *y = k-1;
      p[k] = -1;
      }
    else
      {
      if(i == p[0])
	return(1);
      else
	{
	p[j] = p[i];
	*z = p[i]-1;
	p[i] = 0;
	*x = j-1;
	*y = i-1;
	}
      }
    }
  return(0);
  }

//Initialize p[] to have the following properties
//p[0] = N+1
//  p[1..N-M] = 0
//    p[N-M+1..N] = 1..M
//    p[N+1] = -2
//    if M=0 then p[1] = 1

void inittwiddle(m, n, p)
int m, n, *p;
  {
  int i;
  p[0] = n+1;
  for(i = 1; i != n-m+1; i++)
    p[i] = 0;
  while(i != n+1)
    {
    p[i] = i+m-n;
    i++;
    }
  p[n+1] = -2;
  if(m == 0)
    p[1] = 1;
  }

//function takes 2 floats in an array and compares which is larger, for use in qsort
int compareFloat(const void *a, const void *b) {
   float x1 = *(const float*)a;
   float x2 = *(const float*)b;

   if (x1 > x2) return  1;
   if (x1 < x2) return -1;
   
   return 0;
}	

//function takes 2 doubles in an array and compares which is larger, for use in qsort
int compareDouble(const void *a, const void *b) {
   double x1 = *(const double*)a;
   double x2 = *(const double*)b;

   if (x1 > x2) return  1;
   if (x1 < x2) return -1;

   return 0;
} 

void clear_array(size_t x, size_t y, double array[x][y]){
  	for(int i = 0; i<x; i++){
    	for(int j = 0;j<y; j++){      
      		array[i][j] = (double) 0;
    	}
  	}
}

double findKey(int loc, double kyArr[COL]){
	return kyArr[loc];
}

//fill an empty matrix with all neighboorhoods for a given column
void generate_neighborhood(size_t suburb, size_t street,float cArr[COL][2],double nArr[suburb][street],double kyArr[COL],double rArr[suburb][street]){
  	//Flags used to prevent redundant blocks being generated later
	int sFlag = 0;
	int eFlag = 1;
  	//start at neighborhood 0
	int neighbourhood = 0;
	//for each element in the column
	for(int i = 0;i<(COL-BLOCKSIZE); i++){
		//start flag is the first elemenet of a potential new neighborhood
		sFlag = i;
		
		//if there is a block sized overlap between the last neighborhood and this new one, shift the start of the new neighborhood up so that there is no overlap
		if((eFlag-sFlag)>=BLOCKSIZE){
			i = eFlag-BLOCKSIZE+1;
		}
		
		//this element (i) starts a neighbourhood
		nArr[neighbourhood][0] = findKey(cArr[i][1],kyArr);
		rArr[neighbourhood][0] = cArr[i][1];
		//element to be checked for entry into current neighborhood. (the next element after i)
		int j = i+1;
    	//"distance" between the vlaues in the first element of the neighborhood and the current one
		float dist = (cArr[j][0]-cArr[i][0]);

		//if the new element is within dist, add it to the hood and check the next one etc.
		while(dist<=DIA){
      		//record the value's key and rowID			
			double key = findKey(cArr[j][1], kyArr);
			nArr[neighbourhood][j-i] = key;
		
			rArr[neighbourhood][j-i] = cArr[j][1];
			
			j++;
      		//if there is space in the array, set the next distance, otherwise stop
			if(j-i < street) {
				dist = (cArr[j][0]-cArr[i][0]);
			} else {
				break;
			}
		}
		
		//set end flag as the element after the last element in the old hood
		eFlag = j;
		
		//if this hood is too small to contain blocks, recycle its index in the array
		if(!nArr[neighbourhood][BLOCKSIZE-1] == 0){
			if(neighbourhood<NEIGHBOURHOODNUMBER-1){
				neighbourhood++;
			} else {
        		
        		break;
      		}	
		}	
	}
}
	
//calculate a recursive factorial, for calcualting some N choose R
unsigned long long int fac(unsigned long long int n ){
	if (n >= 1)
        return n*fac(n-1);
    else
        return 1;
}

//find all block combinations within a given neighborhood
void generate_blocks(size_t N,size_t t, double a[N][2], double blockArray[t][BLOCKSIZE+1]){
	//elements in a block
	int M = BLOCKSIZE;
	//block index
	int combination = 1; 
  	//sum of block keys
  	double signature;
	int j = 1;
  	int i, x, y, z, p[N+2], b[N];
  	double c[BLOCKSIZE*2] = {0};
    
    //Generate the first block (rightmost)
  	for(int k = 0; k < M; k++){
  		c[k]=a[N-M+k][0];
  		
  	}
  	for(int k = 0; k < M; k++){
  		c[M+k]=a[N-M+k][1];
  	}
  	signature = 0;

  	for(i = 0; i < BLOCKSIZE; i++){  		
      	signature += c[i];
      	blockArray[0][1+i] = c[BLOCKSIZE+i];
    }

  	blockArray[0][0] = signature;
   	
   	//only one combination required? then return
  	if(N == 1){return;}
	
	//generate all other other combinations
 	inittwiddle(M, N, p);

	//while more combinations still exist
 	while(!twiddle(&x, &y, &z, p)){
   		//write keys to block array
   		c[z] = a[x][0];
      	//write rowIDs to block array
      	c[BLOCKSIZE+z] = a[x][1];
      	//sum the keys within this block and store signature
        signature = 0;

        for(i = 0; i<BLOCKSIZE;i++){
            signature += c[i];
            blockArray[combination][1+i] = c[BLOCKSIZE+i];
        }

        blockArray[combination][0] = signature;
   		//write to the next block if that is where this belongs
   		combination++;	
    }
	
}

//Find all blocks within a column and store them in an array
void generate_blockArray(double bArray[BLOCKARRAYSIZE][1+BLOCKSIZE],double nArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE], double rArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE]){
    int i = 0;
    int block = 0;
    //find blocks in the next hood if there are any hoods left
 	while(nArray[i][0] != 0 && i < NEIGHBOURHOODNUMBER){	
		int j = 0;
		
		//check how big this hood is
		while(nArray[i][j] !=0 && j < NEIGHBOURHOODSIZE){j++;}
		//if bad neighboorhood (last neighboorhood and size less than 3) ignore it
		if(j < BLOCKSIZE){break;}
		//extract key and column info and put into smaller array
		double a[j][2];
		for(int k = 0; k < (j); k++){
			a[k][0] = nArray[i][k];
			a[k][1] = rArray[i][k];
		}
		
		//check number of blocks possible
		unsigned long long int t = fac(j)/(fac(BLOCKSIZE)*fac((j) - BLOCKSIZE));
		//create array that blocks will be stored in. Format: everything doubles [sig,row1,row2,row3,row4]x number of blocks
		double c[t][BLOCKSIZE+1];
		//generate the blocks for this neighborhood
		generate_blocks((j),t, a,c);

		
	  	//store the blocks that have been generated
	    for (int k = 0; k < t; k++) {
        	if(block>=BLOCKARRAYSIZE){break;}
	        for(int l = 0;l<(1+BLOCKSIZE);l++){

	           	bArray[block][l] = c[k][l];
	            
	        }
	            //If a slot in the block array has been filled, fill the next index along with the next value and so on
	          	block++;
	    }	    
	    i++;	   
    }
    //sort the completed block arrray from lowest to highest signature
    qsort(bArray, BLOCKARRAYSIZE, sizeof(*bArray), compareDouble);
}


//Parse data from file into arrays and proccess into blocks
void parse_data(double bArray[BLOCKARRAYSIZE][1+BLOCKSIZE], int column,double keyArray[COL]){
  	float colArray[COL][2] = {0}; //an array for values and one for keys
  	//get the first column SET COLUMN HERE (change to automated after testing)
  	input_data(colArray,column);
    //neighborhood array
  	double neighbArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE] = {0};
    //helper array for neighbor array containing row information
  	double rowArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE] = {0};
    
    //sort the column
    qsort(colArray, COL, sizeof(*colArray), compareFloat);
    //generate all hoods for this column
    generate_neighborhood(NEIGHBOURHOODNUMBER,NEIGHBOURHOODSIZE, colArray, neighbArray, keyArray, rowArray);
    generate_blockArray(bArray,neighbArray,rowArray);
}


//function takes sorted ascending arrays of the blocks generated in two columns, a collision array to output collisions to, and the index of each column being compared
//prints collisions between the two columns
void collisions(double aArr[BLOCKARRAYSIZE][1+BLOCKSIZE], double bArr[BLOCKARRAYSIZE][1+BLOCKSIZE], double collisions[COLLISIONARRAYSIZE][1+BLOCKSIZE], int i, int j){
	//keeps track of total number of collisions
	int collisionTicker = 0;

	//iterates over each signature in first array, starting from the highest 
	for(int i = BLOCKARRAYSIZE-1; i >= 0; i--) {
		//a contains signature being compared
    	double a = aArr[i][0];
      
      	//if signature is zero, reached array space unfilled by blocks
    	if(a == 0) {
        	break;
      	}

      	//iterates over each signature in second (comparison) array, starting from the highest
	    for(int j = BLOCKARRAYSIZE-1; j >= 0; j--) {
	    	//informs loop to save time by stopping comparing signatures when the first signature is higher than the comparison signature
	      	if(a > bArr[j][0]) {
	        	break;
	      	}

	      	//if signatures match
	      	if(a == bArr[j][0]) {
	      		//fill collision matrix with signature and rows (block info)
	        	for(int k = 0; k <= BLOCKSIZE; k++) {
	          		collisions[collisionTicker][k] = aArr[i][k];                
	        	}
	        	//increment number of collisions
	        	collisionTicker++;
	      	}
	   	}
	}

	//print all blocks that collide and the columns theyre found in
  	for(int m = 0; m < collisionTicker; m++) {
		printf("collision %d: sig = %.1f, rows = %.1f, %.1f, %.1f, %.1f, columns = %.1d and %.1d\n", 
    		m+1, collisions[m][0], collisions[m][1],
    		collisions[m][2], collisions[m][3], collisions[m][4],
    		i, j);
  	}  	
}

int main() {
	//struct to contain time values at start and end of execution
 	struct timeval start, end;
 	//get time at start of execution
 	gettimeofday(&start, NULL);
  	
  	int totalCollisions = 0;
 	//array for storing collisions
   	double collisionArray[COLLISIONARRAYSIZE][1+BLOCKSIZE] = {0};
    double outputArray[COLLISIONARRAYSIZE][1+BLOCKSIZE] = {0};
  	//array for storing keys
  	double keyArray[COL] = {0};
  	
  	//get the keys
  	input_key(keyArray);
	
	//inputs from text files
  	//Allocate an array for the blocks from two columns at a time
 	double firstBlockArray[BLOCKARRAYSIZE][1+BLOCKSIZE] = {0};
 	double checkBlockArray[BLOCKARRAYSIZE][1+BLOCKSIZE] = {0};


  	//OPTION: mauanlly set the number of cores to utilised
 	omp_set_num_threads(NUM_THREADS);

  	//OPTION: Allow the maximum number of cores to be utilised
  	//omp_set_num_threads(omp_get_num_threads());

 	int nthreads;
  	
  	//Use a column as a pivot around which to find collisions with all other columns
  	for(int i = 0; i < ROW-1; i++){
   		//generate blocks array for this first column
    	parse_data(firstBlockArray,i, keyArray);

    //begin parallel region
    #pragma omp parallel private(checkBlockArray)
    {
    	int id, nthrds;

    	id = omp_get_thread_num();
    	nthrds = omp_get_num_threads();

    	if(id == 0){
    		nthreads = nthrds;
    	}

    	//for each thread make comparison columns
	    for(int j = (ROW-1) - id; j > i; j = j - nthreads){
	    	//generate second block matrix and compare
	      	parse_data(checkBlockArray,j,keyArray);

	      	//check if there are collisions between 2 columns
		  	collisions(firstBlockArray,checkBlockArray,collisionArray,i,j);

		  	//critical region
		  	#pragma omp critical
		        for(int k = 0; k < COLLISIONARRAYSIZE; k++){
			        if(collisionArray[k][0] == 0){break;}
              		for(int l = 0; l<BLOCKARRAYSIZE+1;l++){
              			//write collisions to output array
			        	outputArray[totalCollisions][l] = collisionArray[k][l];
            		}
		        }
		        totalCollisions++;

		        //clear comparison array and array holding collision between the 2 columns in question
		        clear_array(BLOCKARRAYSIZE,1+BLOCKSIZE,checkBlockArray);
		        clear_array(COLLISIONARRAYSIZE,1+BLOCKSIZE,collisionArray);
	    }
	}

	//clear array held in memory
    clear_array(BLOCKARRAYSIZE,1+BLOCKSIZE,firstBlockArray);
	}

	//get time of day at end of execution
	gettimeofday(&end, NULL);
	//compute time taken in seconds
  	double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
  		 end.tv_usec - start.tv_usec) / 1.e6;

  	//print time program took to execute
  	printf("time = %5.10f seconds\n", delta);
}


//NB Compile instructions
//gcc -fopenmp -o project1 project1.c -std=c99
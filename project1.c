#include <mpi.h>
#include <sys/time.h>
//#include <omp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//number of rows in a block
#define BLOCKSIZE 4
//size of array holding each columns blocks
#define BLOCKARRAYSIZE 10000
//column length in data.txt
#define COL 4400
//size of matrix holding collisions
#define COLLISIONARRAYSIZE 3000
//size of bytes in data file
#define DATACHARS 40000//7000
//diff between values in a neighbourhood
#define DIA 0.000001
//size of bytes in key file
#define KEYCHARS 71000
//max number of neighbourhoods in a given column
#define NEIGHBOURHOODNUMBER 1000
//max size of neighbourhood in given column
#define NEIGHBOURHOODSIZE 100
//optimal number of threads
//#define NUM_THREADS 2
//amount of columns read into program to compute collisions
//NB. max is 499
//NB. row 499 seems impossible to read in, even by itself
//NB. 90 was the max we could get to work on our machines so program would execute and didn't throw the time value to a number with an error in it
#define ROW 20
#define MASTER 0 /* taskid of first task */
#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

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
        		//printf("%s\n", token);
      			arr[j][0] = atof(token);
        		//printf("%f\n", arr[j][0]);
      			arr[j][1] = (float)j;

      			j++;
    			break;
    		}else{i++;}
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

void clear_parray(size_t x, double **array){
  for(int i = 0; i<x; i++){
      free(array[i]);

      array[i] = NULL;
  }
  
  free(array);
  array = NULL;


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
		printf("cArr[%d][0] = %f\n", i, cArr[i][0]);
		//start flag is the first elemenet of a potential new neighborhood
		sFlag = i;
		//if there is a block sized overlap between the last neighborhood and this new one, shift the start of the new neighborhood up so that there is no overlap
		if((eFlag-sFlag)>=BLOCKSIZE){
			i = eFlag-BLOCKSIZE+1;
		}
		//this element (i) starts a neighbourhood
		nArr[neighbourhood][0] = findKey(cArr[i][1],kyArr);
		//printf("neighbourhood = %d, nArr = %f\n", neighbourhood, nArr[neighbourhood][0]);
		rArr[neighbourhood][0] = cArr[i][1];
		//element to be checked for entry into current neighborhood. (the next element after i)
		int j = i+1;
    	//"distance" between the vlaues in the first element of the neighborhood and the current one
		float dist  = (cArr[j][0]-cArr[i][0]);

		//if the new element is within dist, add it to the hood and check the next one etc.
		while(dist<=DIA){
      	//record the value's key and rowID
			
			double key = findKey(cArr[j][1], kyArr);
			nArr[neighbourhood][j-i] = key;
		
			rArr[neighbourhood][j-i] = cArr[j][1];
			
			j++;
      		//if there is space in the array, set the next distance, otherwise stop
			if(j-i < street){
				dist  = (cArr[j][0]-cArr[i][0]);
			}else{
        		printf("Street too small!, i = %d, j = %d\n, cArr[i] = %f, cArr[j] = %f", i, j, cArr[i][0], cArr[j][0]);
				break;
			}
		}
		//set end flag as the element after the last element in the old hood
		eFlag = j;
		//if this hood is too small to contain blocks, recycle its index in the array
		if(!nArr[neighbourhood][BLOCKSIZE-1] == 0){
			if(neighbourhood<NEIGHBOURHOODNUMBER-1){
				neighbourhood++;
			}else{
        		 printf("Hood too small!\n");
        		break;
      		}	
		}	
	}
}
	
//calculate a recursive factorial, for calcualting some N choose R
/*unsigned long long*/ int fac(unsigned long long int n ){
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
  	if(N==1){return;}
	
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
void generate_blockArray(double **bArray,double nArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE], double rArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE]){
    int i = 0;
    int block = 0;
    /*unsigned long long*/ int t;
    //find blocks in the next hood if there are any hoods left
 	while(nArray[i][0] != 0 && i < NEIGHBOURHOODNUMBER){
	
		int j = 0;
		//check how big this hood is
		while(nArray[i][j] !=0 && j < NEIGHBOURHOODSIZE){j++;}
		//if bad neighboorhood (last neighboorhood and size less than 3) ignore it
		if(j < BLOCKSIZE){break;}
		//extract key and column info and put into smaller array
		double a[j][2];
		for(int k = 0; k < j; k++){
			a[k][0] = nArray[i][k];
			a[k][1] = rArray[i][k];
		}
		
		if(j >= BLOCKSIZE) {
			//check number of blocks possible
			t = fac(j)/(fac(BLOCKSIZE)*fac((j) - BLOCKSIZE));
		}else{
			printf("street empty\n");
			break;
		}

		//create array that blocks will be stored in. Format: everything doubles [sig,row1,row2,row3,row4]x number of blocks
		double c[t][BLOCKSIZE+1];
		//generate the blocks for this neighborhood
		generate_blocks((j),t, a,c);

		
	  	//store the blocks that have been generated
	    for (int k = 0; k < t; k++) {
        if(block >= BLOCKARRAYSIZE){
          printf("block too small\n");
          break;}
	        for(int l = 0;l<(1+BLOCKSIZE);l++){
              //printf("%f   ", bArray[block][l]);
	           	bArray[block][l] = c[k][l];
              //printf("%f   ", bArray[block][l]);
	            
	        }
           //printf("\n");
	            //If a slot in the block array has been filled, fill the next index along with the next value and so on
	            
	          	block++;

	    }
	    
	    i++;
	   
    }
    //sort the completed block arrray from lowest to highest signature
    qsort(bArray, BLOCKARRAYSIZE, sizeof(*bArray), compareDouble);
    //printf("start of sorted region\n");
    for(int i = 0; i< BLOCKARRAYSIZE; i++){
      //printf("%d  %f   ",i, bArray[i][0]);
      //printf("%f   \n", bArray[i][4]);

    }
    //printf("end of sorted region\n");
}


//Parse data from file into arrays and proccess into blocks
void parse_data(double **bArray,double keyArray[COL],float colArray[COL][2]){
    //printf("parse %d\n",omp_get_thread_num() );
    //printf("data input %d\n",omp_get_thread_num() );
    //neighborhood array
  	double neighbArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE] = {0};
    //helper array for neighbor array containing row information
  	double rowArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE] = {0};
    //sort the column
    qsort(colArray, COL, sizeof(*colArray), compareFloat);
    //printf("sorted %d\n",omp_get_thread_num() );
    //generate all hoods for this column
    generate_neighborhood(NEIGHBOURHOODNUMBER,NEIGHBOURHOODSIZE, colArray, neighbArray, keyArray, rowArray);
    generate_blockArray(bArray,neighbArray,rowArray);
    //printf("blocks %d\n",omp_get_thread_num() );
    /*
    for(int i =0; i<BLOCKARRAYSIZE;i++){
      printf("%d  %f  %f \n", i, bArray[i][0],bArray[i][4]);
    }
*/
}


//function takes sorted ascending arrays of the blocks generated in two columns, a collision array to output collisions to, and the index of each column being compared
//prints collisions between the two columns
void collisions(double **aArr, double **bArr, double **collisions){
   	//printf("collision %d  %d  %d \n",omp_get_thread_num(), ii, jj );
	//keeps track of total number of collisions
	int collisionTicker = 0;

	//iterates over each signature in first array, starting from the highest 
	//for(int i = BLOCKARRAYSIZE-1; i >= 0; i--) {
  	for(int i = 0; i< BLOCKARRAYSIZE; i++) {
		//a contains signature being compared
     	//printf("first array check %d %d\n",omp_get_thread_num(), i );
    	double a = aArr[i][0];
      	//printf("first array checked %d\n",omp_get_thread_num() );
      	//if signature is zero, reached array space unfilled by blocks
    	if(a == 0) {
        	break;
      	}

      	//iterates over each signature in second (comparison) array, starting from the highest
	    //for(int j = BLOCKARRAYSIZE-1; j >= 0; j--) {
        for(int j = 0; j< BLOCKARRAYSIZE; j++) {
	    	//informs loop to save time by stopping comparing signatures when the first signature is higher than the comparison signature
          	//printf("j = %d\n", j);
	      	if(a > bArr[j][0]) {
	        	break;
	      	}
	      	//if signatures match
          	//printf("column 1 = %d : a = %f, column 2 = %d : %f\n", i, aArr[i][0],j,bArr[j][0]);
           	//printf("check array check %d  %d  \n",omp_get_thread_num(),j );
	      	if(a == bArr[j][0]) {
            //printf("check array checked %d\n",omp_get_thread_num() );

	      		//fill collision matrix with signature and rows (block info)
           	//printf("collide!\n");
	        	for(int k = 0; k <= BLOCKSIZE; k++) {
                    collisions[collisionTicker][k] = aArr[i][k];
                    //printf("collision array changed %d\n",omp_get_thread_num() );

              		// printf("%f\n", collisions[collisionTicker][k]);

                
	        	}
            	// printf("\n");
	        	//increment number of collisions
	        	collisionTicker++;
	      		}
	   	}
	}

	//print all blocks that collide and the columns theyre found in
  	for(int m = 0; m < collisionTicker; m++) {
		printf("collision %d: sig = %.1f, rows = %.1f, %.1f, %.1f, %.1f\n", 
    		m+1, collisions[m][0], collisions[m][1],
    		collisions[m][2], collisions[m][3], collisions[m][4]);
  	}  	
}

int main(int argc, char *argv[]) {
  	printf("MEME\n");
	//struct to contain time values at start and end of execution
 	struct timeval start, end;
 	//get time at start of execution
 	gettimeofday(&start, NULL);

 	//int nthreads; 
	int numtasks,		//numer of tasks
		taskid, 		//task identifier
		//world_size, 	//number of processors
		name_len,		//
		numworkers,  	//worker tasks available
		mtype,			//message type
		source,			//taskid of source
		dest;		//taskid of destination
	double start_time, end_time;	//MPI section timing

	double **collisionArray = (double **)malloc(COLLISIONARRAYSIZE*sizeof(double *));
 	//array for storing collisions
	double outputArray[COLLISIONARRAYSIZE][1+BLOCKSIZE] = {0};
	//array for storing keys
	double keyArray[COL] = {0};
	double **firstBlockArray = (double **)malloc(BLOCKARRAYSIZE*sizeof(double *));
	float colArray[COL][2] = {0}; //an array for values and one for keys
	int totalCollisions = 0;

  	//get MPI status
	MPI_Status status;

	MPI_Init(&argc, &argv);
	//start_time = MPI_Wtime();	//initialise start time
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid); 	//which node is used
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);	//total num of nodes 

	numworkers = numtasks - 1;
	
	//MASTER TASKS
	if(taskid == MASTER) {
		int i, j, k, l, z;	//tickers 	
	 	//array for storing collisions
	    //double outputArray[COLLISIONARRAYSIZE][1+BLOCKSIZE] = {0};
	  	//array for storing keys
	  	//double keyArray[COL] = {0};

	  	//OPTION: manually set the number of cores to utilised
	 	//omp_set_num_threads(NUM_THREADS);

	  	//OPTION: Allow the maximum number of cores to be utilised
	  	//omp_set_num_threads(omp_get_num_threads());
	  	
	  	//get the keys
	  	input_key(keyArray);
		
		//inputs from text files
	  	//Allocate an array for the blocks from two columns at a time
	 	//double firstBlockArray[BLOCKARRAYSIZE][1+BLOCKSIZE] = {0};
	 	//double checkBlockArray[BLOCKARRAYSIZE][1+BLOCKSIZE] = {0};
	  	
	  	//Use a column as a pivot around which to find collisions with all other columns
	  	for(i = 0; i < ROW-1; i++){
	    	printf("%d\n", i);
	        //double **firstBlockArray = (double **)malloc(BLOCKARRAYSIZE*sizeof(double *));
	      	for(j = 0; j<BLOCKARRAYSIZE; j++){  
	      		firstBlockArray[j] = (double *)calloc(1+BLOCKSIZE,sizeof(double));
	     	}
	      	float colArrayPivot[COL][2] = {0};
	      	input_data(colArrayPivot,i);
	   		//generate blocks array for this first column
	    	parse_data(firstBlockArray, keyArray, colArrayPivot);
	      	printf("first array parsed\n");

	    //#pragma omp parallel private(checkBlockArray)
	    //#pragma omp parallel
	    /*{
	    	int id, nthrds;

	    	id = omp_get_thread_num();
	    	nthrds = omp_get_num_threads();

	    	if(id == 0){
	    		nthreads = nthrds;
	    	}

		    for(int j = (ROW-1) - id; j > i; j = j - nthreads){
	        	printf("%d\n",j);
	       		printf("parallel region\n");*/

	       // for(int j = ROW-1; j > i; j = j - numworkers){

				//send every column to worker tasks
				//for(int m = 0; m <= ROW; m = m + numworkers) { //fix this to work with an offset asa well
					//send to worker tasks
          	int checkColumn  = ROW-1;
          	//printf("checkColumn = %d\n", checkColumn);

			mtype = FROM_MASTER;
          	while(checkColumn > i){
          		printf("i = %d\n", i);
          		printf("numworkers = %d\n", numworkers);
				for(dest = 1; dest < numworkers; dest++) {
					printf("dest = %d\n", dest);
		            if(checkColumn > i){
		            	printf("checkColumn = %d\n", checkColumn);
		              	j = checkColumn;
		              	checkColumn--;
		            }else{
		              	break;
		            }

		            //float colArray[COL][2] = {0}; //an array for values and one for keys
		            //get the first column SET COLUMN HERE (change to automated after testing)


		          	//double **fBlockArray = (double **)malloc(BLOCKARRAYSIZE*sizeof(double *));

		              /*for(int k = 0; k<BLOCKARRAYSIZE; k++){  
		                fBlockArray[k] = (double *)calloc(1+BLOCKSIZE,sizeof(double));
		                for(int l =0;l<(1+BLOCKSIZE);l++){
		                    fBlockArray[k][l] = firstBlockArray[k][l];
		                }
		              }*/
		       
		            //double collisionArray[COLLISIONARRAYSIZE][1+BLOCKSIZE] = {0};
		            //double **collisionArray = (double **)malloc(COLLISIONARRAYSIZE*sizeof(double *));
		            for(k = 0; k<COLLISIONARRAYSIZE; k++){  
		                collisionArray[k] = (double *)calloc(1+BLOCKSIZE,sizeof(double));
		            } //change this scope         

		            input_data(colArray,j);


					printf("sending tasks to dest %d\n", dest);
					MPI_Send(&keyArray[0], COL, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
					MPI_Send(&firstBlockArray[0], COL, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
					/*MPI_Send(&collisionArray[0], COLLISIONARRAYSIZE*(1+BLOCKSIZE), 
						MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);*/
					MPI_Send(&colArray[0], COL, MPI_FLOAT, dest, mtype, MPI_COMM_WORLD);
				}

				
          	}
          
			//receive from worker tasks
			mtype = FROM_WORKER;
			for(int p = 1; p <= numworkers; p++) {
				source = p;
				MPI_Recv(&collisionArray[0], COLLISIONARRAYSIZE*(1+BLOCKSIZE), 
					MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
				//MPI_Recv(&collisionTicker, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
				printf("received things from task %d\n", source);
			}

	        for(z = 0; z < COLLISIONARRAYSIZE; z++){
		        if(collisionArray[z][0] == 0){break;}
          		for(l = 0; l<BLOCKSIZE+1;l++){
		        	outputArray[totalCollisions][l] = collisionArray[z][l];
        		}
         		totalCollisions++;
	        }  //output array needs to be chucked in master and mpi_reduced        

        	//printf(" %d\n", j);
        	//printf("free first\n");
        	//clear_parray(BLOCKARRAYSIZE,fBlockArray);

			printf("free true first\n");
			clear_parray(BLOCKARRAYSIZE, firstBlockArray);	
		}

		printf("total collisions = %d\n", totalCollisions);
		//get time of day at end of execution
		gettimeofday(&end, NULL);
		//compute time taken in seconds
	  	double delta = ((end.tv_sec  - start.tv_sec) * 1000000u +
	  		 end.tv_usec - start.tv_usec) / 1.e6;

	  	//print time program took to execute
	  	printf("time = %5.10f seconds\n", delta);
	}

	//WORKER_TASKS
	if(taskid > MASTER) {
		//RECIEVE from master
		mtype = FROM_MASTER;
		MPI_Recv(&keyArray[0], COL, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&firstBlockArray[0], COL, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
		/*MPI_Recv(&collisionArray[0], COLLISIONARRAYSIZE*(1+BLOCKSIZE), 
			MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);*/
		MPI_Recv(&colArray[0], COLLISIONARRAYSIZE*(1+BLOCKSIZE), MPI_DOUBLE, MASTER, 
			mtype, MPI_COMM_WORLD, &status);
		printf("task %d recieves\n", taskid);

 		double **checkBlockArray = (double **)malloc(BLOCKARRAYSIZE*sizeof(double *));
      	for(int k = 0; k<BLOCKARRAYSIZE; k++){  
        	checkBlockArray[k] = (double *)calloc(1+BLOCKSIZE,sizeof(double));
      	}

    	//generate second block matrix and compare
      	parse_data(checkBlockArray,keyArray, colArray);
       	printf("check array parsed\n");

	  	collisions(firstBlockArray,checkBlockArray,collisionArray); //was fBlockArray
    	//printf("collisions complete\n");

	  	//SEND to master
	  	mtype = FROM_WORKER;
	  	MPI_Send(&collisionArray[0], COLLISIONARRAYSIZE*(1+BLOCKSIZE), 
			MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
	  	//MPI_Send(&collisionTicker, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);

      	printf("free check\n");
        clear_parray(BLOCKARRAYSIZE,checkBlockArray);

        printf("free coll\n");
        clear_parray(COLLISIONARRAYSIZE,collisionArray);
        
    }

	MPI_Finalize();

}


//NB Compile instructions
//gcc -fopenmp -o project1 project1.c -std=c99
/*mpicc -fopenmp -o project1 project1.c -std=c99
ssh to linux com sci computer: uggp.csse.uwa.edu.au
ssh to cluster: ecm-ubl-006.uniwa.uwa.edu.au
mpirun -np 2 --hostfile host project1 
mpirun --hostfile host project1
mpirun project1
*/

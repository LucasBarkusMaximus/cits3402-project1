
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
#define ROW 2
#define KEYCHARS 70000
#define DATACHARS 45000
#define BLOCKSIZE 4
#define NEIGHBOURHOODNUMBER 1000
#define NEIGHBOURHOODSIZE 100
#define BLOCKARRAYSIZE 1000
#define COLLISIONARRAYSIZE 1000





void input_data(float arr[COL][2], int colNumber){
  FILE *f;
	int j = 0;
	char str[DATACHARS];
	f = fopen("data.txt", "r");
	while(fgets(str, DATACHARS, f)!=NULL){
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
	fclose(f);
	//testing
	//int a  = sizeof(colArray) / sizeof(float);
	//printf("%d\n", a);
}

void input_key(double arr[COL]){
  FILE *f;
	char str[KEYCHARS];
	f = fopen("keys.txt", "r");

	if(fgets(str, KEYCHARS, f)!=NULL){
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
	fclose(f);
	//testing
	//int a  = sizeof(keyArray) / sizeof(int);
	//printf("%d\n", a);7
}

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

int compareFloat(const void *a, const void *b) {
   float x1 = *(const float*)a;
   float x2 = *(const float*)b;
   if (x1 > x2) return  1;
   if (x1 < x2) return -1;
   return 0;
}	

int compareDouble(const void *a, const void *b) {
   double x1 = *(const double*)a;
   double x2 = *(const double*)b;
   if (x1 > x2) return  1;
   if (x1 < x2) return -1;
   return 0;
} 


double findKey(int loc, double kyArr[COL]){
	return kyArr[loc];
}


void generate_neighborhood(size_t suburb, size_t street,float cArr[COL][2],double nArr[suburb][street],double kyArr[COL],double rArr[suburb][street]){
	int sFlag = 0;
	int eFlag = 1;
	int neighbourhood = 0;
	//for each element in the column
	for(int i = 0;i<(COL-BLOCKSIZE); i++){
		//start flag is the first elemenet of a potential new neighborhood
		sFlag = i;
		//if there is a block sized overlap between the last neighborhood and this new one, shift the start of the new neighborhood up so that there is no overlap
		if((eFlag-sFlag)>=BLOCKSIZE){
			i = eFlag;
		}
		//this element (i) starts a neighbourhood
		nArr[neighbourhood][0] = findKey(cArr[i][1],kyArr);
		rArr[neighbourhood][0] = cArr[0][1];
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
			//printf("row: %d Val:%f \n",j,cArr[j][0]);
			nArr[neighbourhood][j-i] = key;
			rArr[neighbourhood][j-i] = cArr[j][1];
			j++;
			if(j-i< street){
			dist  = (cArr[j][0]-cArr[i][0]);
			//printf("dist:%+8.8f(l)\n", dist);
			}else{
				//printf("rip\n");
				break;
			}
		}
		//set end flag as the element after the last element in the old hood
		eFlag = j;
		//if this hood is too small to contain blocks, recycle its index in the array
		if(!nArr[neighbourhood][BLOCKSIZE-1] == 0){
			neighbourhood++;
			//printf("good neighbourhood\n");
			for(int k=i;k<j;k++){
			//printf("row: %d Val:%lf \n",k,cArr[k][0]);
		}
		}
		
	}

}
	

unsigned long long int fac(unsigned long long int n ){
	if (n >= 1)
        return n*fac(n-1);
    else
        return 1;
}

void generate_blocks(size_t N,size_t t, double a[N][2], double blockArray[t][BLOCKSIZE+1]){
	printf("%s\n","generating blocks" );
	//elements in a block
	int M = BLOCKSIZE;
	//block index
	int combination = 1; 
  	double signature;
	int j = 1;
    int i, x, y, z, p[N+2], b[N];
    double c[BLOCKSIZE*2];
    //Generate the first block (rightmost)
    for(int k = 0;k<M;k++){c[k]=a[N-M+k][0];}
    for(int k = 0;k<M;k++){c[M+k]=a[N-M+k][1];}
    //for(int k = 0;k<M;k++){printf("key:%16.0lf row:%4.0lf\n", c[k],c[BLOCKSIZE+k]);}
       signature = 0;
        for(i = 0; i<BLOCKSIZE;i++){
            signature += c[i];
            blockArray[0][1+i] = c[BLOCKSIZE+i];
         }
         blockArray[0][0] = signature;
    //for(int k = 0;k<M;k++){printf("sig:%16.0lf row:%4.0lf\n", blockArray[0][0],blockArray[0][1+k]);}
   	//only one combination required? then return
    if(N==1){return;}
	//generate other combinations
 	inittwiddle(M, N, p);
  	for(i = 0; i != N-M; i++)  {
   	 b[i] = 0;
   	 putchar('0');
    }
 	 while(i != N) {
   	 b[i++] = 1;
    	putchar('1');
    }
 	putchar('\n');
 	while(!twiddle(&x, &y, &z, p)){
 		//create binary representation of combination (debug)
    	b[x] = 1;
    	b[y] = 0;
    	for(i = 0; i != N; i++)
      	putchar(b[i]? '1': '0');
   		putchar('\n');
   		
   		
   		//printf("combo: %d\n", combination);
   		//write keys to block array
   		c[z] = a[x][0];
      c[BLOCKSIZE+z] = a[x][1];
   		//write rowIDs to block array
   		//c[M+z] = a[x][1];
        //for(int k = 0;k<M;k++){printf("key:%8.8lf row:%8.8lf\n", c[k],c[BLOCKSIZE+k]);}
        signature = 0;
        for(i = 0; i<BLOCKSIZE;i++){
            signature += c[i];
            blockArray[combination][1+i] = c[BLOCKSIZE+i];
         }
         blockArray[combination][0] = signature;
          //for(int k = 0;k<M;k++){printf("sig:%8.8lf row:%8.8lf\n", blockArray[combination][0],blockArray[combination][1+k]);}
   		//write to the next block if that is where this belongs
   		combination++;
   		
   		
   		
    }
	
}


void generate_blockArray(double bArray[BLOCKARRAYSIZE][1+BLOCKSIZE],double nArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE], double rArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE]){
     int i = 0;
     int block = 0;
     //find blocks in the next hood if there are any hoods left
     while(nArray[i][0]!= 0){
      printf("hood: %d\n", i);
      int j = 0;
      //check how big this hood is
      while(nArray[i][j]!=0){j++;}
      //if bad neighboorhood (last neighboorhood and size less than 3) ignore it
      if(j<BLOCKSIZE){break;}
      printf("hood size: %d\n", j);
      //extract key and column in and put into smaller array
      double a[j][2];
      for(int k = 0; k < (j); k++){
        a[k][0] = nArray[i][k];
        a[k][1] = rArray[i][k];
        }
      //check number of blocks possible
      unsigned long long int t = fac(j)/(fac(BLOCKSIZE)*fac((j) - BLOCKSIZE));
      printf("combinations: %d\n", t );
      //create array that blocks will be stored in. Format: everything doubles [sig,row1,row2,row3,row4]x number of blocks
      double c[t][BLOCKSIZE+1];
      //generate
      generate_blocks((j),t, a,c);

       for (int k = 0; k < t; k++) {
        for(int l = 0;l<(1+BLOCKSIZE);l++){
           bArray[block][l] = c[k][l];
           block++;
          }
        }
      i++;
     }

      qsort(bArray, 1000, sizeof(*bArray), compareDouble);
      for (int i = 0; i < 1000; ++i){
         //printf("(%lf) \n", bArray[i][0]);
      }
}

void parse_data(double bArray[BLOCKARRAYSIZE][1+BLOCKSIZE], int column,double keyArray[COL]){
  float colArray[COL][2]; //an array for values and one for keys
  //get the first column SET COLUMN HERE (change to automated after testing)
  input_data(colArray,column);
  
  double neighbArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE];
  double rowArray[NEIGHBOURHOODNUMBER][NEIGHBOURHOODSIZE];
  
  /*
  printf("OG: \n");
    for(int i = 0; i < COL; i++) {
      printf("(%f, %f) \n", colArray[i][0], colArray[i][1]);
    }
*/
    //sort the column
    qsort(colArray, COL, sizeof(*colArray), compareFloat);

   /*
    printf("Sorted: \n");
    for (int i = 0; i < COL; i++) {
      printf("(%f, %f) \n", colArray[i][0], colArray[i][1]);
    }
   */

    //generate all hoods for this column
     generate_neighborhood(NEIGHBOURHOODNUMBER,NEIGHBOURHOODSIZE, colArray, neighbArray, keyArray, rowArray);
    /*
     while(neighbArray[i][0]!= 0){
      int j = 0;
      while(neighbArray[i][j]!=0){j++;}
        if(j<BLOCKSIZE){break;}
        comb += fac(j)/(fac(BLOCKSIZE)*fac((j) - BLOCKSIZE));
        printf("%d\n", comb);
      i++;
     }
     blockArray[comb][1+BLOCKSIZE];
     i = 0;
*/
      generate_blockArray(bArray,neighbArray,rowArray);
}

void collisions(double aArr[BLOCKARRAYSIZE][1+BLOCKSIZE], double bArr[BLOCKARRAYSIZE][1+BLOCKSIZE], double collisions[COLLISIONARRAYSIZE][1+BLOCKSIZE]){
	int collisionTicker = 0;

	for(int i = BLOCKARRAYSIZE-1; i >= 0; i--) {
		double a = aArr[i][0];
		printf("a = %f\n", a);
		printf("%d element = %f\n", i, aArr[BLOCKARRAYSIZE-1][0]);
		printf("0 element = %f\n", aArr[0][0]);
    	
    	if(a == 0) {
    		printf("broke after = %f\n", aArr[i + 1][0]);
    		break;
    	}

		for(int j = BLOCKARRAYSIZE-1; j >= 0; j--) {
			/*if(a > bArr[j][0]) {
				break;
			}*/

			if(a == bArr[j][0]) {
				for(int k = 0; k <= BLOCKSIZE; k++) {
					collisions[collisionTicker][k] = aArr[i][k];
				}

				collisionTicker++;
			}
		}
	}

	for(int m = 0; m <= collisionTicker; m++) {
		printf("collision %d: sig = %f, rows = %f, %f, %f, %f\n", 
			collisionTicker, collisions[m][0], collisions[m][1],
			collisions[m][2], collisions[m][3], collisions[m][4]);
	}

	printf("collisionTicker = %d\n", collisionTicker);

}

int main() {
 	struct timeval start, end;
 	gettimeofday(&start, NULL);

  	//array for storing a hood
  
 	double collisionArray[COLLISIONARRAYSIZE][1+BLOCKSIZE] = {0};
  	//array for storing keys
  	double keyArray[COL];
  	//get the keys
  	input_key(keyArray);
	//inputs from text files
 	double firstBlockArray[BLOCKARRAYSIZE][1+BLOCKSIZE];
 	double checkBlockArray[BLOCKARRAYSIZE][1+BLOCKSIZE];
  	for(int i = 0;i<ROW;i++){
    	if(i!=0){
      		for(int j =0;j<BLOCKARRAYSIZE;j++){
        		for(int k = 0;k<BLOCKSIZE+1;k++) firstBlockArray[j][k] = checkBlockArray[j][k];
      		}
    	}else{
      		parse_data(firstBlockArray,0, keyArray);
    	}
    	for(int j = ROW-1;j>i;j--){
      		parse_data(checkBlockArray,j,keyArray);
	   	 	collisions(firstBlockArray,checkBlockArray,collisionArray);
      	}
	}
     
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
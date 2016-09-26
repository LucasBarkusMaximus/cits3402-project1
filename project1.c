#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIA 0.000001

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



	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	return 0;
}
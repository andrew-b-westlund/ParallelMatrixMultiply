#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>

int n;
int p;

int ind(int r, int c) {
    return (r*n + c);
}

double msDiffTime(struct timespec start, struct timespec stop) {
    long seconds = stop.tv_sec - start.tv_sec;
    long ns = stop.tv_nsec - start.tv_nsec;

    if(ns < 0) {
    	seconds--;
        ns += 1000000000;
	}
    return (1000.0 * ((double)seconds + (double)ns/(double)(1000000000)));
}

void trans(double*a, int n) {
	for(int r=0; r<n; r++) {
		for(int c=r; c<n;c++) {
			double t;
			t=a[ind(r,c)];
			a[ind(r,c)] = a[ind(c,r)];
			a[ind(c,r)] = t;
		}
	}
}

double dotProd(double *a, double *b){
	// fprintf(stderr, "In dotProd\n");
	// double *sumArray = (double *)malloc(p * sizeof(double));
	// for (int a = 0; a < p; a++) {
		// sumArray[a] = 0.0;
	// }
	// fprintf(stderr, "pragma omp parallel for num_threads(%d)", p);
	double sum = 0;
	// #pragma omp parallel for reduction(+:sum) num_threads(p)
	for (int i = 0; i < n; i++) {
		
		// #pragma omp critical
		// {
			sum += a[i] * b[i];
		// }
		// fprintf(stderr, "Thread: %d\n", omp_get_thread_num());
		// sumArray[omp_get_thread_num()] += a[i] * b[i];
	}
	
	// for (int a = 0; a < p; a++) {
		// sum += sumArray[a];
	// }
	return sum;
}

void mm(double *A, double *B, double *C) {
	
    int i,j,k;
    // double sum;
	// fprintf(stderr, "In mm\n");
//	#pragma omp parallel private(sum,i,j,k) 
// {
	#pragma omp parallel for private(i, j) schedule(static, 32)
    for(i=0; i < n; i++) {
		// #pragma omp parallel for private(j)
        for(j=0; j< n; j++) {
			C[ind(i,j)] = dotProd(&A[ind(i, 0)], &B[ind(j,0)]);
		}
    }
	// }
}

int main(int argc, char *argv[]) {
	int numOfSizeTrials = 0, numOfThreadTrials = 0;
	int *sizeTrials, *threadTrials;
	#ifdef LOOP
	{
		numOfSizeTrials = 4;
		numOfThreadTrials = 7;
		// sizeTrials = {100, 1000, 4000, 6000};
		
		sizeTrials = (int *)malloc(4 * sizeof(int));
		sizeTrials[0] = 100;
		sizeTrials[1] = 1000;
		sizeTrials[2] = 4000;
		sizeTrials[3] = 6000;
		
		// threadTrials = {1, 2, 4, 8};
		threadTrials = (int *)malloc(7 * sizeof(int));
		threadTrials[0] = 1;
		threadTrials[1] = 2;
		threadTrials[2] = 4;
		threadTrials[3] = 8;
		threadTrials[4] = 16;
		threadTrials[5] = 24;
		threadTrials[6] = 32;
	}
	#else
	{
		if(argc < 2) {
			fprintf(stderr, "Usage: mm n p - where n is the dimension, and p is the number of threads \n");
			exit(-1);
		}
		sscanf(argv[1], "%d", &n);
		// fprintf(stderr, "n = %d\n", n);

		if (argc >= 2) {
			sscanf(argv[2], "%d", &p);
		} else {
			p = 1;
		}
		// fprintf(stderr, "p = %d\n", p);
		numOfSizeTrials = 1;
		numOfThreadTrials = 1;
		// sizeTrials = {0};
		// threadTrials = {0};
		sizeTrials = (int *)malloc(1 * sizeof(int));
		sizeTrials[0] = n;
		threadTrials = (int *)malloc(1 * sizeof(int));
		threadTrials[0] = p;
	}
	#endif
	
	for (int x = 0; x < numOfSizeTrials; x++){
	for (int y = 0; y < numOfThreadTrials; y++){
	n = sizeTrials[x];
	p = threadTrials[y];
	fprintf(stderr, "n = %d\n", n);
	fprintf(stderr, "p = %d\n", p);
    double *A, *B, *C, *I;
    int i,j;
    struct timespec tStart, tStop;
    int nomatch;

    //pointers point at the start of a chunk of memory, the beginning of our Array
    A = (double *)malloc(n*n*sizeof(double));
    B = (double *)malloc(n*n*sizeof(double));
    C = (double *)malloc(n*n*sizeof(double));
    I = (double *)malloc(n*n*sizeof(double));

    for(i=0; i < n; i++) {
	for(j=0; j < n; j++) {
	    A[ind(i,j)] = 0.4;
	    B[ind(i,j)] = 0.8;
	    
	    if (i == j) {
		I[ind(i,j)] = 1.0;
	    } else {
	        I[ind(i,j)] = 0;
	    }
	}
    }

    clock_gettime(CLOCK_MONOTONIC, &tStart);
    trans(B, n);
    mm(A, I, C);
    trans(B, n);
    clock_gettime(CLOCK_MONOTONIC, &tStop);
    

    fprintf(stderr, "mm took %6.3fms for a %d squared matrix \n", msDiffTime(tStart, tStop), n);

    nomatch = 0;
    for(i=0;i<n;i++) {
	for(j=0; j<n; j++) {
	    if(A[ind(i,j)] != (C[ind(i,j)])) {
		nomatch = 1;
	    }
	}
    }
    if (nomatch) {
	fprintf(stderr, "Did not match \n");
    }
	}
	}
}

# ParallelMatrixMultiply
Multiplies a Matrix with Multiple Threads

The software will cycle through multiple tests with varying numbers of threads and matrix sizes. mm5 parallelizes the outer-most loop of the matrix multiplication, mm3 parallelizes the dotProduct function, and mm4 parallelizes the inner loop. These programs are used to display the differences between parallelizing different parts of the same program, as well as to what extent that a greater number of threads is beneficial to the speed of the program.

Future versions of the programs will incorporate the use of the MPI. 

Input: If LOOP is defined, no input necessary; otherwise, the first number, n, will determine the number of rows and columns the matrices will have, and the second number, p, will determine the number of threads to be used: n is necessary, but if p is not declared, it will default to 1.

Output: The lengths of time the matrix multiplication(s) take to run in ms.

Compile: gcc mm#.c -O2 -o mm# (-D LOOP), where # is which program you want to run.

Execute: ./mm# (n p)

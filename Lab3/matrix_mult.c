/**
 * @file matrix_mult.c
 * @author Kweku Andoh Yamoah(kweku.yamoah@ashesi.edu.gh)
 * @brief This file will cotain an implementation of matrix multiplication naively and another implementation using the MPI library
 * @version 0.1
 * @date 2021-03-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdlib.h>
#include <stdio.h>


/**
 * @brief N represents matrix size
 * N=128
 * N=1024
 * N=2048
 * N=4096
 */
#define N 128
#define N1 1024
#define N2 2048
#define N3 4096

/*
Defining prototypes for function
*/
void matrixInitialise(int **, int);
void matrixDisplay(int **, int);
int **matrixMemoryAllocate(int);


int matrix_size;//global variable used by diagonal pthreads
int matrix_size= N; //work around to make matrix size globally accesible

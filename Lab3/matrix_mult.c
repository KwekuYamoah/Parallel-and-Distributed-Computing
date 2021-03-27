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

/**
 * @brief dynamicall creates a 2 dimensional array
 * of any size
 * 
 * @param size 
 * @return int** 
 */
int **matrixMemoryAllocate(int size){
    //allocate space
    int **matrix;
    matrix = malloc(size * sizeof(int*));

    for (int i = 0; i < size; i++){
        matrix[i] = malloc(size * sizeof(int));
    }
    
    /*if allocation failed*/
    if(matrix == NULL){
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    /*if successful*/
    return matrix;

}

/**
 * @brief initialise matrix 
 * 
 * @param matrix 
 * @param size 
 */
void matrixInitialise(int **matrix, int size){
    int k = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j ++){
           k ++;
           matrix[i][j] = k;
        }
    }
}

/**
 * @brief display matrix contents
 * 
 * @param matrix 
 * @param size 
 */
void matrixDisplay(int **matrix, int size){
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j ++){
           printf("%d \t", matrix[i][j]);
        }

        printf("\n");
    }
}
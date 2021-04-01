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

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


/**
 * @brief N represents matrix size
 * N=128
 * N=1024
 * N=2048
 * N=4096
 */
#define N 8
#define N1 1024
#define N2 2048
#define N3 4096

int **A;
int **B;
int **C; //Three Matrices

/*
Defining prototypes for function
*/
void matrixInitialise(int **, int);
void matrixInitialiseZeros(int **, int);
void matrixDisplay(int **, int);
int **matrixMemoryAllocate(int);
void matrixMultiplicationNaive(int **,int **,int **, int);
void matrixMultiplicationMPI(int **, int **, int **, int);


int matrix_size;//global variable used by diagonal pthreads
int matrix_size= N; //work around to make matrix size globally accesible


/**
 * @brief main function to run code
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */

void printarr(int **data, int n, char *str);
int **allocarray(int n);

int main(int argc, char *argv[]){

    /*time_t work_time;

    /*Create Matrix A,B,C and initilise it
    A = matrixMemoryAllocate(matrix_size);
    B = matrixMemoryAllocate(matrix_size);
    C = matrixMemoryAllocate(matrix_size);

    //initialisation of matrices
    matrixInitialise(A, matrix_size);
    matrixInitialise(B, matrix_size);
    matrixInitialiseZeros(C, matrix_size);

    
    /*Compute A*B
    work_time = clock();
    matrixMultiplicationNaive(A , B ,C ,matrix_size);
    work_time = clock() - work_time;
    printf("Time taken by matrixMultiplicationNaive(): %f s\n\n", 
    ((double)work_time)/CLOCKS_PER_SEC);

    /**
     * @brief Free memory of matrices
     * after sequential execution
     * 
     

    free(B);
    free(C); */

    /**
     * @brief MPI Implementation of Matrix multiplication
     * 
     */
    int myrank, P; // rank of current process and no. of processes
    int part_rows, part_columns, i, j, k;
    int tag = 100;

    const int sender  =0;
    const int receiver=1;


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    part_rows = sqrt(P); //Will be passed to matrix A and partitioned into sqrt(P) rows
    part_columns = sqrt(P); //Will be passed to matrix B and partitioned into sqrt(P) columns

    /**
     * @brief Check if our row partitions and column 
     * partitions divide our matrix size evenly.This is
     * an essential assumption for our algorithm to work
     * 
     */
    if( matrix_size % part_columns !=0 && matrix_size % part_rows != 0){
         fprintf(stderr,"%s: Only works with np=%d for now\n", argv[0], part_rows);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

     //Time to create row and column datatypes
    MPI_Datatype columntype, tmp;
    MPI_Status stat;

    /**
     * @brief if process is p0 initialise
     * Matrices A, B & C
     * 
     */
    if(myrank == sender){
        A = matrixMemoryAllocate(matrix_size);
        matrixInitialise(A, matrix_size);
        

        MPI_Datatype rowtype;
        int starts[2] = {0,0};
        int subsizes[2] = {P, P};
        int bigsizes[2] = {matrix_size, matrix_size};

        MPI_Type_create_subarray(2, bigsizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &rowtype);
        MPI_Type_commit(&rowtype);

        MPI_Send(&(A[0][0]), 1, rowtype, receiver, tag, MPI_COMM_WORLD);
        MPI_Type_free(&rowtype);

        free(A[0]);
        free(A);

    }
    else if(myrank ==1){
        C = matrixMemoryAllocate(P);
        matrixInitialiseZeros(C, P);
        matrixDisplay(C,P);
        printf("\n \n");

        MPI_Recv(&(C[0][0]), P*P, MPI_INT, sender, tag, MPI_COMM_WORLD, &stat);

        matrixDisplay(C,P);


        free(C[0]);
        free(C);
    }

   MPI_Finalize();
   return (EXIT_SUCCESS);
}



/**
 * @brief dynamically creates a 2 dimensional array
 * of any size
 * 
 * @param size 
 * @return int** 
 */
int **matrixMemoryAllocate(int n){
    //allocate space
    int *data = malloc(n*n*sizeof(int));
    int **arr = malloc(n*sizeof(int *));
    for (int i=0; i<n; i++)
        arr[i] = &(data[i*n]);

    
    
    /*if allocation failed*/
    if(arr == NULL){
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    /*if successful*/
    return arr;

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
 * @brief initialise matrix with only zeros
 * 
 * @param matrix 
 * @param size 
 */
void matrixInitialiseZeros(int **matrix, int size){
    int k = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j ++){
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

/**
 * @brief Computes A*B sequentially
 * 
 * @param matrix 
 * @param size 
 */
void matrixMultiplicationNaive(int **matrix_a,int **matrix_b,int **matrix_c, int size){
    int i,j,k;
    for(i = 0; i < size; i++){
        for(j= 0; j < size; j++){
            for(k = 0; k < size; k++){
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
}


void matrixMultiplicationMPI(int **matrix_a, int **matrix_b, int **matrix_c, int size){

    

}
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

int **local_row_matrix;
int **local_column_matrix;
int **local_mult_matrix ;
int **temp;

/*
Defining prototypes for function
*/
void matrixInitialise(int **, int);
void matrixInitialiseZeros(int **, int);
void matrixInitialise2(int **, int, int);
void matrixInitialiseZeros2(int **, int, int);
void matrixDisplay(int **, int);
int matrixMemoryAllocate(int ***,int);
void matrixDisplay2(int **, int, int);
int matrixMemoryAllocate2(int ***,int, int);

void matrixMultiplicationNaive(int **,int **,int **, int);
void matrixMultiplicationMPI(int **, int **, int **, int);

MPI_Datatype createSubarrayRows(int, int);
MPI_Datatype createSubarrayColumns(int, int);

void scatterRows(int **, int **, int , int , int , int , MPI_Datatype);
void scatterColumns(int **, int **, int , int , int ,int , MPI_Datatype );


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

    time_t work_time;

    //Create Matrix A,B,C and initilise it
    matrixMemoryAllocate(&A,matrix_size);
    matrixMemoryAllocate(&B,matrix_size);
    matrixMemoryAllocate(&C,matrix_size);

    //initialisation of matrices
    matrixInitialise(A, matrix_size);
    matrixInitialise(B, matrix_size);
    matrixInitialiseZeros(C, matrix_size);

    
    //Compute A*B
    work_time = clock();
    matrixMultiplicationNaive(A , B ,C ,matrix_size);
    work_time = clock() - work_time;
    printf("Time taken by matrixMultiplicationNaive(): %f s\n\n", 
    ((double)work_time)/CLOCKS_PER_SEC);

    printf("Matrix C from serial: \n");
    matrixDisplay(C, matrix_size);
    printf("\n \n");

    /**
     * @brief Free memory of matrices
     * after sequential execution
     * 
     *
    **/
     
    free(A);
    free(B);
    free(C); 

    /**
     * @brief MPI Implementation of Matrix multiplication
     * 
     */
    int myrank, P; // rank of current process and no. of processes
    int part_rows, part_columns, i, j, k;
    int tag = 100;

   
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


    /**
     * @brief if process is p0 initialise
     * Matrices A, B & C
     * 
     */

    if(myrank == 0){
        //Initialise Array and B
        matrixMemoryAllocate(&A,matrix_size);
        matrixInitialise(A, matrix_size);
        

        matrixMemoryAllocate(&B,matrix_size);
        matrixInitialise(B, matrix_size);

        matrixMemoryAllocate(&C,matrix_size);
        matrixInitialiseZeros(C, matrix_size);

    }
    //Initialise local array
    matrixMemoryAllocate2(&local_row_matrix,matrix_size, matrix_size);
    matrixInitialiseZeros2(local_row_matrix, matrix_size, matrix_size);

    matrixMemoryAllocate2(&local_column_matrix,matrix_size, matrix_size);
    matrixInitialiseZeros2(local_column_matrix,matrix_size, matrix_size );

    matrixMemoryAllocate2(&local_mult_matrix,matrix_size, matrix_size);
    matrixInitialiseZeros2(local_mult_matrix,matrix_size, matrix_size);
   
    matrixMemoryAllocate2(&temp,matrix_size, matrix_size);
    matrixInitialiseZeros2(temp, matrix_size, matrix_size);


    //Time to create row and column datatypes
   
    MPI_Datatype rowtype = createSubarrayRows(part_rows, matrix_size);
    MPI_Datatype columntype = createSubarrayColumns(part_columns, matrix_size);

    if(myrank == 0){
        printf("*************************************************\n");
        printf("Scattering Elements in matrix A to a local array \n \n \n");
    }

    //scatter rows
    scatterRows(A, local_row_matrix, matrix_size, part_rows,  myrank, P, rowtype);
    MPI_Barrier(MPI_COMM_WORLD);

    if(myrank == 0){
        printf("*************************************************\n");
        printf("Scattering Elements in matrix B to a local array \n \n \n");
    }
    //scatter columns
    scatterColumns(B, local_column_matrix, matrix_size, part_columns, myrank, P, columntype);
    MPI_Barrier(MPI_COMM_WORLD);

    /**
     * @brief Now let me see your shouting
     * we are doing the multiplication 
     * 
     */
    //first re-oriente column matrix
    //transpose source matrix before scattering
    
    k=0;
    int q =0;
    for(i =0; i <part_columns; i ++){
        for(j = 0; j < matrix_size; j+= 2){
            temp[q][k] = local_column_matrix[i][j];
            q++;
            
        }
        
    }

    k++;
    q=0;
    for(i =0; i <part_columns; i ++){
        for(j = 1; j < matrix_size; j+= 2){
            temp[q][k] = local_column_matrix[i][j];
            q++;
            
        }
        
    }

   


    for(i = 0; i < matrix_size; i++){
        for(j= 0; j < matrix_size; j++){
            for(k = 0; k < matrix_size; k++){
                local_mult_matrix[i][j] += local_row_matrix[i][k] * local_column_matrix[k][j];
            }
        }
       
    }


    /**
     * @brief Gathering results from local matrix
     * to matrix C
     * 
     */
    const int sender = 0;

    int *globalptr = NULL;

    if(myrank == 0){
        globalptr = &(C[0][0]);
    }
    int send_counts[matrix_size/part_rows];
    int dispals[matrix_size/part_rows];
    int grid_size = matrix_size/part_rows;
    if(myrank == 0){
    
        for(i = 0; i < grid_size; i ++){
            send_counts[i]= 1;
        }

        //calculate displacements
        k = 0;
        for(i = 0; i < grid_size; i ++){ 
            //dispals[k] = i *(part_rows * matrix_size);
            dispals[i] = k;
            k+=part_rows;
        }
    }


    MPI_Gatherv(&(local_mult_matrix[0][0]), part_rows * matrix_size, MPI_INT, globalptr, send_counts, dispals, rowtype ,sender, MPI_COMM_WORLD);

    //free local memory
    free(local_column_matrix);
    free(local_mult_matrix);
    free(local_row_matrix);

    //free MPI Datatypes
    MPI_Type_free(&rowtype);
    MPI_Type_free(&columntype);

    if(myrank == 0){ 
        matrixDisplay(temp, matrix_size);
        printf("\n \n");
        printf("Matrix C results MPI implementation : \n");
        matrixDisplay2(C, matrix_size, matrix_size);
    }
    
    MPI_Finalize();
    return (EXIT_SUCCESS);
}

MPI_Datatype createSubarrayRows(int partition, int size){
        MPI_Datatype type, tmp;
        int starts[2] = {0,0};
        int subsizes[2] = {partition, size};
        int bigsizes[2] = {size, size};

        MPI_Type_create_subarray(2, bigsizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &tmp);
        MPI_Type_create_resized(tmp, 0, matrix_size*sizeof(int), &type);
        MPI_Type_commit(&type);

        return type;
}

MPI_Datatype createSubarrayColumns(int partition, int size){
        MPI_Datatype type, tmp;
        int starts[2] = {0,0};
        int subsizes[2] = {size, partition};
        int bigsizes[2] = {size, size};

        MPI_Type_create_subarray(2, bigsizes, subsizes, starts, MPI_ORDER_C, MPI_INT, &tmp);
        MPI_Type_create_resized(tmp, 0, partition*sizeof(int), &type);
        MPI_Type_commit(&type);

        return type;
}

void scatterRows(int **source_matrix, int **dest_matrix, int matrix_size, int part_rows, int myrank, int P, MPI_Datatype rowtype){
    int i,j,k;
    const int sender = 0;

    int *globalptr = NULL;

    if(myrank == 0){
        globalptr = &(source_matrix[0][0]);
    }
    int send_counts[matrix_size/part_rows];
    int dispals[matrix_size/part_rows];
    int grid_size = matrix_size/part_rows;

    if(myrank == 0){
    
        for(i = 0; i < grid_size; i ++){
            send_counts[i]= 1;
        }

        //calculate displacements
        k = 0;
        for(i = 0; i < grid_size; i ++){ 
            //dispals[k] = i *(part_rows * matrix_size);
            dispals[i] = k;
            k+=part_rows;
        }

        printf("\n");
    }


    
    MPI_Scatterv(globalptr, send_counts, dispals, rowtype, &(dest_matrix[0][0]),part_rows * matrix_size, MPI_INT, sender, MPI_COMM_WORLD);

    /* now all processors print their local data: */
    /**for(j = 0; j < P; j ++){
        if(myrank == 0){
            printf("Local process on rank %d is: \n", myrank);
            matrixDisplay2(dest_matrix, part_rows, matrix_size);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }**/
}

void scatterColumns(int **source_matrix, int **dest_matrix, int matrix_size, int part_columns, int myrank, int P, MPI_Datatype columntype){
    int i,j,k;
    const int sender = 0;

    int *globalptr = NULL;

    if(myrank == 0){
        globalptr = &(source_matrix[0][0]);
    }
    int send_counts[matrix_size/part_columns];
    int dispals[matrix_size/part_columns];
    int grid_size = matrix_size/part_columns;

    if(myrank == 0){
    
        for(i = 0; i < grid_size; i ++){
            send_counts[i]= 1;
        }

        //calculate displacements
        k = 0;
        for(i = 0; i < grid_size; i ++){ 
            dispals[i] = k;
            k+=1;
        }
        printf("\n");
    }
    
    MPI_Scatterv(globalptr, send_counts, dispals, columntype, &(dest_matrix[0][0]),part_columns * matrix_size, MPI_INT, sender, MPI_COMM_WORLD);

    /* now all processors print their local data: */
    /**for(j = 0; j < P; j ++){
        if(myrank == 0){
            printf("Local process on rank %d is: \n", myrank);
            matrixDisplay2(dest_matrix, part_columns, matrix_size);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }**/
}


/**
 * @brief dynamically creates a 2 dimensional array
 * of any size
 * 
 * @param size 
 * @return int** 
 */
int matrixMemoryAllocate(int ***array,int n){
    //allocate space
    int *data =(int *)malloc(n*n*sizeof(int));
    
    if(!data){return -1;}
    (*array) = (int **)malloc(n*sizeof(int *));

    if(!(*array)){
        free(data);
        return -1;
    }

    for (int i=0; i<n; i++)
       (*array)[i] = &(data[i*n]);

    return 0;
    
    

}

int matrixMemoryAllocate2(int ***array ,int row, int column){
    //allocate space
    int *data =(int *)malloc(row*column*sizeof(int));
    
    if(!data){return -1;}
    (*array) = (int **)malloc(row*sizeof(int *));

    if(!(*array)){
        free(data);
        return -1;
    }

    for (int i=0; i<row; i++)
       (*array)[i] = &(data[i*row]);

    return 0;
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

void matrixInitialise2(int **matrix, int row, int column){
    int k = 0;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j ++){
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

void matrixInitialiseZeros2(int **matrix, int row, int column){
    int k = 0;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j ++){
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

void matrixDisplay2(int **matrix, int row, int column){
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j ++){
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
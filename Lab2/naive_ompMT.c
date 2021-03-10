/**
 * @file naive_ompMT.c
 * @author Kweku Yamoah (kweku.yamoah@ashesi.edu.gh)
 * @brief A naive OpenMp threaded algorithm to parallelise code for matrix transpose
 * @version 0.1
 * @date 2021-03-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */


/*
Including header files
*/

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4

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
void naiveOMPTranspose(int **, int);
void diagonalOMPTranspose(int **, int);
int **matrixMemoryAllocate(int);
void transposeBasic(int **, int);
void swap(int **, int, int);
void *diagonalPthread(void *);



int **matrix; //global matrix variable to be used in main


int matrix_size;//global variable used by diagonal pthreads
int matrix_size= N; //work around to make matrix size globally accesible
/**
 * 
 * @brief threads are generated for each diagonal elements for matrix transpose
 * 
 * @param rank 
 * @return void* 
 */
void *diagonalPthread(void *rank){
    int start,end, myRank;
    myRank = (int) rank;

    start = myRank * (matrix_size/NUM_THREADS);
    end = (myRank + 1) * (matrix_size/NUM_THREADS);


    for(int i = start; i < end; i++){
        for(int j= i + 1; j < matrix_size; j ++){
            swap(matrix, i, j);
        }
    }

    pthread_exit(NULL);
}

/**
 * @brief auxillary method to help acheive diagonal threading 
 * with pthreads
 * 
 * @param num_threads 
 * @param start_routine 
 */
void run_threads(int num_threads, void *(*start_routine)(void *)){
    pthread_t threads[num_threads];
    int thread, t;

    for(t = 0; t < num_threads; t++){  

        //create the threads. 
        thread = pthread_create(&threads[t], NULL, start_routine, 
                (void *) t);
            
        if(thread){
            fprintf(stderr, "Error: connot create threads\n");
            exit(-1);
        }
    }
    //Join all threads
    for( t =0; t < num_threads; t++){
        pthread_join(threads[t], NULL);
    } 
}

/**
 * @brief swaps values of matrix at given indices
 * 
 * @param matrix 
 * @param i 
 * @param j 
 */
void swap(int **matrix, int i, int j){
    
    int temp = matrix[i][j];
    matrix[i][j]= matrix[j][i];
    matrix[j][i] = temp; 
}

/**
 * @brief main function to run code
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]){
    time_t work_time;
    /*Create matrix, initialise and print it.*/
    matrix = matrixMemoryAllocate(matrix_size);
    matrixInitialise(matrix, matrix_size);
    //printf("Matrix :\n");
    //matrixDisplay(matrix,matrix_size);
    
    /* Basic matrix transpose.*/
    work_time = clock();
    transposeBasic(matrix, matrix_size);
    work_time = clock() - work_time;
    printf("Time taken by transposeBasic(): %f s\n\n",
    ((double)work_time)/CLOCKS_PER_SEC);

    transposeBasic(matrix, matrix_size);

    /*OpenMp Naive Threaded Algorithm*/
    work_time = clock();
    naiveOMPTranspose(matrix,matrix_size);
    work_time = clock() - work_time;
    printf("Time taken by naiveOMPTranspose(): %f s\n\n", 
    ((double)work_time)/CLOCKS_PER_SEC);

    naiveOMPTranspose(matrix,matrix_size); //Transpose matrix back to original form

    /*Diagonal Threading Algorithm-Pthreads*/
    
    work_time = clock();
    run_threads(NUM_THREADS, diagonalPthread);
    work_time = clock() - work_time;
    printf("Time taken by diagonalPthread: %f s\n\n", ((double)work_time)/CLOCKS_PER_SEC);

    run_threads(NUM_THREADS, diagonalPthread);
    
    /* Diagonal Threading Algorithm-OpenMP*/
    work_time = clock();
    diagonalOMPTranspose(matrix,matrix_size);
    work_time = clock() - work_time;
    printf("Time taken by diagonalOMPTranspose(): %f s\n\n", ((double)work_time)/CLOCKS_PER_SEC);

    diagonalOMPTranspose(matrix,matrix_size);

    free(matrix); //free space

    return (EXIT_SUCCESS);
}

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

/**
 * @brief naively transposes a matrix by parallelising
 * the for loops using OpenMP
 * 
 * @param matrix 
 * @param size 
 */
void naiveOMPTranspose(int **matrix, int size){
    int nThreads;

    #pragma omp parallel
    {
        int id;
        id = omp_get_thread_num();


        /*Master thread can do this*/
        if(id == 0){
            nThreads = omp_get_num_threads();
        
        }

        #pragma omp for nowait
        for(int i = 0; i < size; i ++){
            for(int j=i + 1; j< size; j ++){
                swap(matrix, i, j);
            }
        }
    } /*Parallel section ends*/
}

void diagonalOMPTranspose(int **matrix, int size){
    int nThreads;
    omp_set_dynamic(0);  // Explicitly disable dynamic teams
    omp_set_num_threads(4); // Use 4 threads for all consecutive parallel regions
    #pragma omp parallel
    {
        int id,start,end,i,j;
        
        id = omp_get_thread_num();
        
        nThreads = omp_get_num_threads();

        start = id * (size/nThreads);
        
        end = (id + 1) * (size/nThreads);
        
        for(i = start; i < end; i++){
            for(j= i + 1; j < size; j ++){
                
                swap(matrix, i, j);
            }  
        }
    } /*Parallel section ends*/

}

/**
 * @brief basic matrix transpose with no parallelism
 * 
 * @param matrix 
 * @param size 
 */
void transposeBasic(int **matrix, int size){
    for(int i = 0; i < size; i ++){
        for(int j=i+1; j< size; j ++){
            swap(matrix, i, j);
        }
    }
  
}
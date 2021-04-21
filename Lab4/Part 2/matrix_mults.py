"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""
from generate_matrix import generateMatrix
import numpy as np
import sys
import time

def matrixMultiply(n):
   
    """
    This function performs the serial multiplication 
    of a matrix. Runtime measurements are taken for 
    a comparision with the MapReduce version
    """

    """
    Generate an N x N matrix A
    and B
    """
    matrix_a = generateMatrix(int(n))
    matrix_b = generateMatrix(int(n))

    #generate an matrix of zeros for matrix c
    matrix_c = np.zeros(int(n) * int(n))
    matrix_c = matrix_c.reshape(int(n), int(n)) #reshape to get rows x cols matrix
    

    #iterate through rows of matrix a
    for i in range(len(matrix_a)):
        #iterate through columns of matrix b
        for j in range(len(matrix_b[0])):
            #iterate through rows of matrix b
            for k in range(len(matrix_b)):
                matrix_c[i][j] += matrix_a[i][k] * matrix_b[k][j]
    

    #do matrix multiplication with numpy
    #matrix_c = np.matmul(matrix_a, matrix_b)
   
    

    
    #write results to text file
    with open('matrixSerialOut.txt', 'w') as testfile:
        for row in matrix_c:
            testfile.write('\t'.join([str(int(a)) for a in row]) + '\n')
            
    
    
if __name__ == '__main__':
    start = time.time()
    args = sys.argv
    globals()[args[1]](*args[2:])
    end = time.time()
    print("--- %s s---" % str(round((end - start),4)))

#run with "time python3 matrix_mults.py matrixMultiply 500 500"
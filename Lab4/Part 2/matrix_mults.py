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

    #do multiplication
    #note rows = cols
    
    for i in range(int(n)):
        for j in range(int(n)):
            for k in range(int(n)):
                matrix_c[i][j] = matrix_c[i][j] + matrix_a[i][k] + matrix_b[k][j]
    

    
    #write results to text file
    file_c = open("matrixC_serial.txt", 'w+')
    file_c.write("%d %d\t\n" % (int(n),int(n)))
    np.savetxt(file_c, matrix_c, delimiter='\t', fmt='%d')

    file_c.close()
if __name__ == '__main__':
    args = sys.argv
    start_time = time.time()
    globals()[args[1]](*args[2:])
    print("--- %s minutes ---" % str((time.time() - start_time)/60))

#run with "time python3 matrix_mults.py matrixMultiply 500 500"
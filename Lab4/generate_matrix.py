"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""
import numpy as np

def generateMatrix(rows, cols):
    """
    This function generates an N * N 
    matrix and writes the results to two
    files matrixA.txt and matrixB.txt
    """

    #open files for writing
    file_a = open("matrixA.txt", 'w+')
    file_b = open("matrixB.txt","w+")

    matrix = np.arange(1, (rows * cols) + 1)
    matrix = matrix.reshape(rows,cols)


    
    np.savetxt(file_a, matrix, delimiter='\t', fmt='%d')
    np.savetxt(file_b, matrix, delimiter='\t', fmt='%d')

    
    file_a.close()
    file_b.close()

generateMatrix(10,10)
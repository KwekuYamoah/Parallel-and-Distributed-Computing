"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""
import numpy as np
import sys

def generateMatrix(rows, cols):
    """
    This function generates an N * N 
    matrix and writes the results to two
    files matrixA.txt and matrixB.txt
    """

    #open files for writing
    file_a = open("matrixA.txt", 'w+')
    file_b = open("matrixB.txt","w+")

    #write matrix dimensions to files
    file_a.write("%d %d\t\n" % (int(rows),int(cols)))
    file_b.write("%d %d\t\n" % (int(rows),int(cols)))

    matrix = np.arange(1, (int(rows) * int(cols) + 1))
    matrix = matrix.reshape(int(rows),int(cols))


    
    np.savetxt(file_a, matrix, delimiter='\t', fmt='%d')
    np.savetxt(file_b, matrix, delimiter='\t', fmt='%d')

    
    file_a.close()
    file_b.close()

    return matrix

if __name__ == '__main__':
    args = sys.argv
    globals()[args[1]](*args[2:])

#run with "python3 generate_matrix.py generateMatrix rows cols"
"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""
import numpy as np
import sys

def generateMatrix(n):
    """
    This function generates an N * N 
    matrix and writes the results to two
    files matrixA.txt and matrixB.txt as row col values
    """

    #open files for writing
    file_a = open("matrixA.txt", 'w+')
    file_b = open("matrixB.txt","w+")

    #write matrix dimensions to files
    file_a.write("%d %d\t\n" % (int(n),int(n)))
    file_b.write("%d %d\t\n" % (int(n),int(n)))

    matrix = np.zeros(int(n) * int(n))
    matrix = matrix.reshape(int(n),int(n))

    i = 1
    for m in range(0,int(n)):
        for o in range(0,int(n)):
            matrix[m][o] = i
            i+=1

    
    for i in range(0, int(n)):
        for j in range(0,int(n)):
            file_a.write(str(i) + ' ' + str(j) + ' ' + str(matrix[i][j]) + '\n')
            file_b.write(str(i) + ' ' + str(j) + ' ' + str(matrix[i][j]) + '\n')


    
    file_a.close()
    file_b.close()

    return matrix

if __name__ == '__main__':
    args = sys.argv
    globals()[args[1]](*args[2:])

#run with "python3 generate_matrix.py generateMatrix rows cols"
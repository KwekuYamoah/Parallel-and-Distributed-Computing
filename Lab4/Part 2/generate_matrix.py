"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""
import sys

def generateMatrix(n):
    """
    This function generates an N * N 
    matrix and writes the results to two
    files matrixA.txt and matrixB.txt as row col values
    """

    #open files for writing
    file_a = open("matrix.txt", 'w+')
    

    matrix = [[0 for i in range(int(n))] for j in range(int(n))] #create matrix
    

    a = 1
    for m in range(0,int(n)):
        for o in range(0,int(n)):
            matrix[m][o] = int(a)  
            a+=1 
            

    
    with open('matrix.txt', 'w') as testfile:
        for row in matrix:
            testfile.write('\t'.join([str(a) for a in row]) + '\n')
            

    
    file_a.close()
   
    return matrix

if __name__ == '__main__':
    args = sys.argv
    globals()[args[1]](*args[2:])

#run with "python3 generate_matrix.py generateMatrix rows cols"
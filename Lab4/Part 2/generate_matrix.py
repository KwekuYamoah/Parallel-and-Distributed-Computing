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
    

    #write matrix dimensions to files
    file_a.write("%d %d\t\n" % (int(n),int(n)))

    matrix = [[0 for i in range(int(n))] for j in range(int(n))] #create matrix
    

    a = 1
    for m in range(0,int(n)):
        for o in range(0,int(n)):
            matrix[m][o] = int(a)  
            a+=1 
            

    
    for i in range(0, int(n)):
        for j in range(0,int(n)):
            file_a.write(str(i) + ' ' + str(j) + ' ' + str(int(matrix[i][j])) + '\n')
            

    
    file_a.close()
   
    return matrix

if __name__ == '__main__':
    args = sys.argv
    globals()[args[1]](*args[2:])

#run with "python3 generate_matrix.py generateMatrix rows cols"
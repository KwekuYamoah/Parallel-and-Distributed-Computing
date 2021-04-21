"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""

from mrjob.job import MRJob
from matrix_dimensions import getDimensions


import math
import re
import os
import time

WORD_RE = re.compile(r"[\w']+")
matA = getDimensions("matrixA.txt")
matB = getDimensions("matrixB.txt")

i = int(matA[0])
j = int(matA[1])
k = int(matB[1])

counter = 0

class MrMatrixReduce(MRJob):
    """Class performs a parallel matrix
    multiplication with sqrt(p) partitions

    Args:
        MrJob ([type]): gotten fromm the job class
    """

    file_c = open('matrixC.txt', 'w+')

    #read dimensions of matrices by using one file
    file = open("matrixA.txt","r+")
    line = file.readline()
    n = list(line.split())[0] #dimensions of matrices

    

    def configure_args(self):
        """This method configures command line arguments for the program. 
        It allows a user to specify the number of processors to utilise for block partitioning
        """
        super(MrMatrixReduce, self).configure_args()
        self.add_passthru_arg(
            "--P", 
            help = "specify the number of processors. "
        )


    def mapper(self,_,line):
        #make counter global
        global counter



        x = line.split()
        if len(x) == 2:
            return
        row, col, val = line.split()
        
        listA = []
        listB = []

        filename = os.environ['mapreduce_map_input_file']

        row = int(row)
        col = int(col)
        val = int(val)
        
        if 'A' in filename:
            listA.append((row, col, val))
            #do partition and send row values within that range with cols as keys for matrix A
            for b_block in range(k):
                yield (row,b_block), ('A', col, val)
            
        if 'B' in filename:
            listB.append((row, col, val))
            #do partition and send col values within that range with rows as keys for matrix  B
            for a_block in range(i):
                yield (a_block,col),('B',row,val)
            

    def reducer(self, key, values):
        listA = [0]*int(j)
        listB = [0]*int(j)
        listFinal = []
       

        for val in values:
            if val[0] == 'A':
                position = val[1]
                listA[position]= val[2]
            elif val[0] =='B':
                position = val[1]
                listB[position]= val[2]
        
        for x  in range(0, len(listA)):
            listFinal.append(listA[x] * listB[x])
        total = sum(listFinal)
        yield key, total
        self.file_c.write(str(key[0]) + " " + str(key[1]) + " ")
        self.file_c.write(str(total) + "\n")
        
    
   

if __name__ == '__main__':
    start = time.time()
    MrMatrixReduce.run()
    end = time.time()
    print(end-start)

#run with "time python3 mrjob_matrixmult.py matrixA.txt matrixB.txt"
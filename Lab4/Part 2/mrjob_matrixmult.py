"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""

from mrjob.job import MRJob
from mrjob.step import MRStep


import math
import re
import os
import time

WORD_RE = re.compile(r"[\w']+")

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


    def mapper_one(self,_,line):
        #make counter global
        global counter



        x = line.split()
        x = list(map(int,x))

        if len(x) == 3:
            row, col, val = x
        elif len(x) == 2 and counter == 1:
            row, val = x
            col = 0
        elif len(x) == 2 and counter == 0:
            counter = 1
            return
        
        
        A_BLOCK_SIZE = int(self.n)/math.sqrt(int(self.options.P))
        B_BLOCK_SIZE = A_BLOCK_SIZE
        filename = os.environ['mapreduce_map_input_file']

        if 'A' in filename:
            
            for b_block in range(int(int(self.n)/math.sqrt(int(self.options.P)))):
                yield col, ('A', row, val)
            
        if 'B' in filename:
            
            for a_block in range(int(int(self.n)/math.sqrt(int(self.options.P)))):
                yield row,('B',col,val)
            

    def reducer_one(self, keys, values):
        
        listA = []
        listB = []

        for val in values:
            if val[0] == 'A':
                listA.append(val)
            if val[0] =='B':
                listB.append(val)
        
        for row0,col0,val0 in listA:
            for row1,col1,val1 in listB:
                yield (col0, col1), int(val0)*int(val1)

    def mapper_two(self, key, value):
        yield key, value
    
    def reducer_two(self, key, values):
        
        total = sum(values)
        yield key, total
        self.file_c.write(str(key[0]) + " " + str(key[1]) + " ")
        self.file_c.write(str(total) + "\n")
        
    
    def steps(self):
        return [MRStep(mapper=self.mapper_one,
                       reducer=self.reducer_one),
                MRStep(mapper=self.mapper_two,
                       reducer=self.reducer_two)] 

if __name__ == '__main__':
    start = time.time()
    MrMatrixReduce.run()
    end = time.time()
    print(end-start)

#run with "time python3 mrjob_matrixmult.py matrixA.txt matrixB.txt"
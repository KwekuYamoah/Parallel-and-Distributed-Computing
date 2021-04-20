"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""

from mrjob.job import MRJob
from mrjob.step import MRStep

import re
import os
import time
WORD_RE = re.compile(r"[\w']+")

class MrMatrixReduce(MRJob):
    """Class performs a parallel matrix
    multiplication with sqrt(p) partitions

    Args:
        MrJob ([type]): gotten fromm the job class
    """

    file_c = open('matrixC.txt', 'w+')

    def steps(self):
        return [MRStep(mapper=self.mapper_one,
                       reducer=self.reducer_one),
                MRStep(mapper=self.mapper_two,
                       reducer=self.reducer_two)]  
    
    def mapper_one(self,_,line):
        x = line.split()
        if len(x) == 2:
            return
        
        row, col,val = line.split()

        row = int(row)
        col = int(col)
        val = int(val)

        filename = os.environ['map_input_file']

        if filename == 'matrixA.txt':
            yield col, ('A', row, val)
        
        if filename =='matrixB.txt':
            yield row,('B',col,val)
            # print(i,('B',j,val))

    def reducer_one(self, j, values):
        listA = []
        listB = []

        for val in values:
            if val[0] == 'A':
                listA.append(val)
            if val[0] =='B':
                listB.append(val)
        
        for(a,i,val1) in listA:
            for(b,k,val2) in listB:
                yield (i,k),int(val1)*int(val2)

    def mapper_two(self, col, value):
        yield (col),value
    
    def reducer_two(self, key, value):
        total = sum(value)
        yield key, total
        self.file_c.write(str(total) + "\n")

if __name__ == '__main__':
    start = time.time()
    MrMatrixReduce.run()
    end = time.time()
    print(end-start)
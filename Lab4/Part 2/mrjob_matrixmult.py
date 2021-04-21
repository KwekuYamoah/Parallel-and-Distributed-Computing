"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""

from mrjob.job import MRJob
import mrjob


import math
import time
import generate_partitions


class MrMatrixReduce(MRJob):
    """Class performs a parallel matrix
    multiplication with sqrt(p) partitions

    Args:
        MrJob ([type]): gotten fromm the job class
    """

    file_c = open('matrixOutput.txt', 'w+')

    #read dimensions of matrices by using one file
    file = open("matrix.txt","r+")
    line = file.readline()
    n = list(line.split())[0] #dimensions of matrices

    INPUT_PROTOCOL = mrjob.protocol.RawProtocol

    

    """def configure_args(self):
        This method configures command line arguments for the program. 
        It allows a user to specify the number of processors to utilise for block partitioning
        
        super(MrMatrixReduce, self).configure_args()
        self.add_passthru_arg(
            "--P", 
            help = "specify the number of processors. "
        )""" 


    def mapper_raw(self,input_file,input_uri):
        matrix =[]
        count = 1

        with open(input_file,'r') as file: 
            for line in file.readlines():
                row  = [int(index) for index in line.split()]
                matrix.append(row)
        
        for i in generate_partitions.rowMajor(matrix,8,2):
            for j in generate_partitions.columnMajor(matrix,8,2):
                yield str(count), i
                yield str(count), j

                count += 1

    def reducer(self, key, values):

        count = 0
        localList = []
        answer = []

        for i in values:
            localList +=[i]
        for row in localList[:1][0]:
            for col in localList[1:][0]:
                answer.append(generate_partitions.calculate(row,col))
        
        yield key, answer
    
if __name__ == '__main__':
    start = time.time()
    MrMatrixReduce.run()
    end = time.time()
    print(end-start)

#run with "time python3 mrjob_matrixmult.py matrixA.txt matrixB.txt"
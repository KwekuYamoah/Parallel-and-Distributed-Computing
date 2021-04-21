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

    

    def configure_args(self):
        """This method configures command line arguments for the program. 
        It allows a user to specify the number of processors to utilise for block partitioning
        """ 
        super(MrMatrixReduce, self).configure_args()
        self.add_passthru_arg(
            "--P", 
            help = "specify the number of processors. "
        )

    n = sum(1 for line in open('matrix.txt')) #get dimensions of matrix
    

    def mapper_raw(self,input_file,input_uri):
        """Maps partions into key vale pairs from a file

        Args:
            input_file (text): file containing matrix
            input_uri (string): path of file based on
            number of processors defined

        Yields:
            [int]: multiplication of matrix given
        """
        matrix =[]
        count = 1

        with open(input_file,'r') as file: 
            for line in file.readlines():
                row  = [int(index) for index in line.split()]
                matrix.append(row)
        offset = int((self.n/math.sqrt(int(self.options.P))))
        for i in generate_partitions.rowMajor(matrix,8,offset):
            for j in generate_partitions.columnMajor(matrix,8,offset):
                yield str(count), i
                yield str(count), j

                count += 1

    def reducer(self, key, values):
        """Aggregates the results from the mapper
           and yields a local multiplication for
           that block

        Args:
            key ([type]): [description]
            values ([type]): [description]

        Yields:
            [type]: [description]
        """

        count = 0
        localList = []
        answer = []

        for i in values:
            localList +=[i]
        for row in localList[:1][0]:
            for col in localList[1:][0]:
                answer.append(generate_partitions.calculate(row,col))
        
        yield key, answer
        self.file_c.write(str(key) + " ")
        self.file_c.write(str(answer) + "\n")

        
            
    
if __name__ == '__main__':
    start = time.time()
    MrMatrixReduce.run()
    end = time.time()
    print(end-start)

#run with "time python3 mrjob_matrixmult.py matrix.txt --P=P"
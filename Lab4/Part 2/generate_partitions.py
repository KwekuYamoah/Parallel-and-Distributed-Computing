def rowMajor(matrix,p,offset):
    """Partition a given matrix into row-major order

    Args:
        matrix (array(list<list>)): an n x n matrix
        p (int): sqrt(P) == number of processors
        offset (int): number of blocks=N/sqrt(p)

    Yields:
        array: partitioned block in row-major order
    """
    i=0
    while i < p:
        start = i * offset
        end = (i*offset) + offset
        yield matrix[start:end]
        i+=1

def columnMajor(matrix, p, offset):
    """Partition a given matrix into column-major order

    Args:
        matrix (array(list<list>)): an n x n matrix
        p (int): sqrt(P) == number of processors
        offset (int): number of blocks=N/sqrt(p)

    Yields:
        array: partitioned block in column-major order
    """
    k = 0
    while k < p:
        array = []
        start = k * offset
        end = (k*offset) + offset

        for i in range(len(matrix)):
            array += matrix[i][start:end]

        yield cleanColumn(array,offset)
        k+=1

def cleanColumn(column,offset):
    """Reorients a given column

    Args:
        column (list): block of numbers
        offset (int): integer displacements

    Returns:
        list: reoriented column
    """
    split =[[] for i in range(offset)]

    for i in range(len(column)):
        split[i%offset].append(column[i])
    return split

def calculate(row,column):
    """Performs local block multiplication

    Args:
        row (array): partitioned row
        column (array): partitioned column

    Returns:
        array: local results
    """

    answer = 0
    for count in range(len(row)):
        answer += (row[count]*column[count])
    return answer
def getDimensions(name):
    """Gets the rows and columns of a matrix from a text file

    Args:
        name (string): filename for matrix

    Returns:
        tuple: a tuple of the rows and columns
    """
    file = open(name, 'r')

    size = file.readline().split() #split at a tab
    rows = size[0]
    cols = size[1]

    file.close()

    print(rows,cols)

    return (rows, cols)
getDimensions("matrixA.txt")
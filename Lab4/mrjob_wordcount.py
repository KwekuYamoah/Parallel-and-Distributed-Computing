"""
@author: Kweku Andoh Yamoah
@id: 71712022
@version: 1.0.0
"""


from mrjob.job import MRJob
from mrjob.step import MRStep
import re 

class MRWordCount(MRJob):
    """
    This program counts the number times
    a word appears in a text file passed
    """

    def configure_args(self):
        """
        This method configures the commandline
        options for the program.
        The commandline options:
            --stop-words: final containing the stop words
        """
        super(MRWordCount, self).configure_args()
        self.add_file_arg(
            "--stop-words", 
            help = "specify the stop words file. "
        )
    def mapper_init(self):
        """
        This method initializes or setup 
        the resources needed by the mapper function. 
        """
        file = self.options.stop_words
        self.stopwords_set = None
        with open(file, "r") as f:
            words = [ i.strip().lower() for i in f.read().split()]
        self.stopwords_set = set(words)
    
    def mapper(self, _, line):
        """
        This method splits every sentence from 
        the text file is split into words. All
        stop words are excuded and the finally 
        the a key-value pairs of (word, 1) are 
        returned. 
        """
        #yield each non stopword in the line.
        for word in WORD_RE.findall(line):
            if word not in self.stopwords_set:
                yield (word.lower(), 1)
    
    def combiner(self, word, counts):
        """
        This method combines the count
        of every word in the file through a yield 
        """
        yield (word, sum(counts))
    
    def reducer(self, word, counts):
        """
        This method aggregates the ouput from
        the mapper to do the final work. It takes
        key-value pairs as an input and returns 
        sum of the values. THe values are the 
        subset of the number of times the key
        appears in the text. 
        """
        #sum all the words we've seen so far
        yield(word, sum(counts))

if __name__ == "__main__":
    MRWordCount.run()

from mrjob_wordcount import MRWordCount 
import sys 

##set up command line arguements here... 

def lab4(job):
    if job == "count":
        print("do word count")
    else:
        print("program not supported")


if __name__=="__main__":
    job = sys.argv[1]
    lab4(job)
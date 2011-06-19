
import sys

id = sys.argv[1]
#file = sys.argv[2]
file = "jformatFilesTop950/all.top950.jformat"

for line in open(file):

    fields = line.split(";")
    classID = fields[2];
    lineID = fields[0]

    if(lineID == id):
        print classID.split("=")[1]
        exit




import sys


def getClass(id):

    for line in open(file):

        fields = line.split(";")
        classID = fields[2];
        lineID = fields[0]

        if(lineID == id):
            return classID.split("=")[1]



fileNet = "top950.blast.ok3.sorted"

#file = sys.argv[2]
file = "jformatFilesTop950/all.top950.jformat"

iguais = 0
linha = 0

for line in open(fileNet):
    linha += 1
    fields = line.split();

    if getClass(fields[0]) == getClass(fields[1]):
        iguais+=1    
        print iguais, " em " , linha


print "existem ", iguais, " iguas nas ", linha, " linhas do arquivo"


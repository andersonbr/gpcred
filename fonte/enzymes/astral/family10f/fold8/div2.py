import sys
import random
import os

if len(sys.argv) <= 1:
	print "Please enter a filename as first argument and a porcentage as second and third ones."
#    print "python treinoValTeste.py file porcent_treino porcent_validacao"
	exit(1)


filename = sys.argv[1]

file = open(filename)
random.seed(1)

numfolds = 2

fold = {}

fold[0] = open("_treino", "w")
fold[1] = open("_validacao", "w")

proctreino = 70
procval = 30

for line in file:

    r = random.randint(1,100)

    if r  <= proctreino:
        print >> fold[0], line,
    else:
        print >> fold[1], line,
        

# elif r <= (3 * porc):
#        print >> fold[2], line,
#    elif r <= (4 * porc):
#        print >> fold[3], line,
#    else:
#        print >> fold[4], line,


for f in range(0,numfolds):
    fold[f].close()



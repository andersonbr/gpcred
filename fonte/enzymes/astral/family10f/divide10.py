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

numfolds = 10

fold = {}
lines = {}
for f in range(0,numfolds):
    fold[f] = open("_fold" + str(f), "w")


porc = 100/numfolds
for line in file:

    r = random.randint(1,100)

    for f in range(0, numfolds):
        if r  <= ((f + 1) * porc):
            print >> fold[f], line,
            break

# elif r <= (3 * porc):
#        print >> fold[2], line,
#    elif r <= (4 * porc):
#        print >> fold[3], line,
#    else:
#        print >> fold[4], line,


for f in range(0,numfolds):
    fold[f].close()



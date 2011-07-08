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

numfolds = 5

fold = {}
lines = {}
for f in range(0,numfolds):
    fold[f] = open("_fold" + str(f), "w")


porc = 100/numfolds
for line in file:

    r = random.randint(1,100)

    if r <= (1 * porc) :
        print >> fold[0], line,
    elif r <= (2 * porc):
        print >> fold[1], line,
    elif r <= (3 * porc):
        print >> fold[2], line,
    elif r <= (4 * porc):
        print >> fold[3], line,
    else:
        print >> fold[4], line,


for f in range(0,numfolds):
    fold[f].close()
    fold[f] = open("_fold" + str(f), "r")
    lines[f] = fold[f].readlines()


#gererate files...
treino = {}
teste = {}
validacao = {}
for f in range(0,numfolds):
    os.system("rm -f _treino"+str(f) + " _validacao"+str(f) + " _teste"+str(f))

for f in range(0,numfolds):
    treino[f] = open("_treino" + str(f), "a")
    teste[f] = open("_teste" + str(f), "a")
    validacao[f] = open("_validacao" + str(f), "a")

#fullfil files
for f in range(0,numfolds):
    for line in lines[f]:
        print >>teste[f], line,
        
for f in range(0,numfolds):        
    for line in lines[(f+1)%numfolds]:
        print >>validacao[f], line,
    
for f in range(0,numfolds):        
    for f2 in range(0,numfolds-2):        
        for line in lines[(f2 + f + 2) % numfolds]:
            print >>treino[f], line,



for f in range(0,numfolds):
    os.system("rm _fold"+str(f))

 


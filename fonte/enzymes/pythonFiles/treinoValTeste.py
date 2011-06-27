import sys
import random

if len(sys.argv) <= 3:
	print "Please enter a filename as first argument and a porcentage as second and third ones."
#    print "python treinoValTeste.py file porcent_treino porcent_validacao"
	exit(1)


filename = sys.argv[1]
treinoporc = int(sys.argv[2])
validacaoporc = int(sys.argv[3])

file = open(filename)
treino = open("_treino", "w")
validacao = open("_validacao", "w")
teste = open("_teste", "w")

random.seed(1)

for line in file:
    r = random.randint(1,100)
    
#    print r, treinoporc, validacaoporc

    if r < treinoporc:
        print "printed"
        print >> treino, line,

    elif r < (validacaoporc + treinoporc):
        print >> validacao, line,

    else:
        print >> teste, line,




import sys
import os
import re

fold= 0 # int(os.getcwd().split('fold')[1])

with open('cred.best') as credfile:
    line = (list(credfile)[-2])

indparentesis = (line.split(":")[1]).strip()

p = re.compile(r'\(|\)')
ind = p.sub('',indparentesis)

runcmd="./gp-cred"

evalin = open('eval.in','w')
print >>evalin, ind
evalin.close()

#treino: 
#credcmd = "./gp-cred -treino ../../../../colecoes/uci/cat/nursery/fold%d//_treino%d.jformat -teste ../../../../colecoes/uci/cat/nursery/fold%d//_validacao%d.jformat -dontsave -cat 8 -knn 10 -eval eval.in eval.out" % (fold, fold, fold, fold)

base="../../../fonte/enzymes/astral/family10pAll"
treinopath= base + "/treino/_treino" + str(fold)
validacaopath= base + "/validacao/_validacao" + str(fold)
testepath= base+ "/teste/_teste" + str(fold)
graph = base+"/outfile.astral.blast.ok4"

#teste
credcmd = "./gp-cred -dontoptimize -treino " + treinopath + " -validacao " + validacaopath + " -teste " + testepath + " -dontsave -num 15 -knn 1 -eval eval.in eval.out -g 1 " + graph

print credcmd
#os.system(credcmd)



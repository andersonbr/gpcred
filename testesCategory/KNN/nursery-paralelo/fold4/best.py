import sys
import os
import re

fold= int(os.getcwd().split('fold')[1])

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

#teste
credcmd = "./gp-cred -treino ../../../../colecoes/uci/cat/nursery/fold%d//_treino%d.jformat -validacao ../../../../colecoes/uci/cat/nursery/fold%d//_validacao%d.jformat -teste ../../../../colecoes/uci/cat/nursery/fold%d//_teste%d.jformat -dontsave -cat 8 -knn 10 -eval eval.in eval.out" % (fold, fold, fold, fold, fold, fold)

os.system(credcmd)




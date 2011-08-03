
import sys
import os

file = open(sys.argv[1])

for l in file:
    metricName = l.strip("\n") 
    if not metricName:
        continue
    
    evalfile = open("evalfile.in", "w")

    os.system("echo "+str(metricName)+" > evalfile.in")

    for i in range(0,5):

        evalFileNameout = metricName+str(i)

        command = "./gp-cred -treino ../../../../arquivos/acm/CV5/treino/treino" +str(i)+" -teste ../../../../arquivos/acm/CV5/validacao/validacao"+str(i) + " -noterms -dontoptimize -g 2 ../../../graphs/authorship.txt ../../../graphs/citation.txt -eval evalfile.in "+evalFileNameout
        
        print i, metricName, evalFileNameout, command
        os.system(command)

        os.system("rm cred.best cred.hist final.out predictions.out")

    evalfile.close()
    os.system("rm evalfile.in")


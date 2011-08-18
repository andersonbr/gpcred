
import sys

treino = open(sys.argv[1])
val = open(sys.argv[2])
rel = open(sys.argv[3])
teste = open(sys.argv[4])

mapaTest = {}
numPerClassTest = {}
predictedClassTest = {}
hits = {}
classes = []
relacao = []

for line in rel:
    relacao.append(line)

def carregaTreino(fileIn, mapIdClass):

    for line in fileIn:
    #    print line

        fields = line.split(";")

        id = fields[0]
        classid = fields[2]
    #   print id,classid
        mapIdClass[id] = classid
        
        if classid not in classes:
            classes.append(classid)

def carregaTeste():
    lista = []
    for line in teste:
        fields = line.split(";")
        id = fields[0]
        classid = fields[2]
        lista.append(id)
        
        if classid not in classes:
            classes.append(classid)
        mapaTest[id] = classid

    return lista

def carregaRelacao(mapa, lista):
    
    acertei = 0
    errei = 0

    for l in lista:
        correta = mapaTest[l]
        if correta in numPerClassTest:
            numPerClassTest[correta] += 1
        else:
            numPerClassTest[correta] = 1


#        print l, correta

        votes = {}

        for line in relacao:
            if l in line:
                (id1, id2, weight) = line.split(" ")

#                print id1, id2, l
                if id1 == l:
                    other = id2
                else:
                    other = id1

                classOther = mapa.get(other,-5)
                
                if classOther == -5:
                    continue

                if not classOther in votes:
                    votes[classOther] = 1 #float(weight)
                else:
                    votes[classOther] += 1 #float(weight)
	
        max = 0
        escolhida = -1
        for (c,v) in votes.iteritems():
            if v > max:
                max = v
                escolhida = c

        if correta == escolhida:
            acertei += 1
            if escolhida in hits:
                hits[escolhida] += 1
            else:
                hits[escolhida] = 1
        else:
            errei += 1

        if escolhida in predictedClassTest:
            predictedClassTest[escolhida] += 1
        else:
            predictedClassTest[escolhida] = 1

        
        print l, correta, escolhida, max, acertei, errei
        
    print "Avaliacao final =>>> acertei =  ", acertei , "  errei = " , errei
    microF1 = float(acertei / float(acertei + errei))
    print "MicroF1 = " , microF1
    
    macroF1 = 0.0
    contaClasse = []

    for c in classes:
        precision = 0.0
        recall = 0.0
        f1 = 0.0
        
        if c not in numPerClassTest:
            numPerClassTest[c] = 0.0
        if c not in hits:
            hits[c] = 0.0
        if c not in predictedClassTest:
            predictedClassTest[c] = 0.0

        if numPerClassTest[c] > 0:
            recall = float(hits[c]) / float(numPerClassTest[c])
            if c not in contaClasse:
                contaClasse.append(c)
        if predictedClassTest[c] > 0:
            precision = float(hits[c]) / float(predictedClassTest[c])
            if c not in contaClasse:
                contaClasse.append(c)
        if precision+recall > 0:
            f1 = float(2.0*precision*recall / float(precision+recall))

        macroF1 += float(f1)
        print "class = " , c , " macro = ", macroF1, " f1 = " , f1, " precision = " , precision, " recall = ", recall

    print "macroF1 = " , macroF1, " contaClasses = " , len(contaClasse)
    macroF1 = float( macroF1 / len(contaClasse) )
    print "MacroF1 = " , macroF1
    
    outfile = open("final.out", "w")
    print >>outfile, "Test", microF1, macroF1
    outfile.close()


mapa = {}
carregaTreino(treino,mapa)
carregaTreino(val,mapa)
lista = carregaTeste()
carregaRelacao(mapa, lista)



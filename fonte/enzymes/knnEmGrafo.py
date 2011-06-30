
import sys

treino = open(sys.argv[1])
rel = open(sys.argv[2])
teste = open(sys.argv[3])

mapaTest = {}

relacao = []

for line in rel:
    relacao.append(line)


def carregaTreino():

    mapIdClass = {}

    for line in treino:
    #    print line

        fields = line.split(";")

        id = fields[0]
        classid = fields[2]
    #   print id,classid
        mapIdClass[id] = classid

    return mapIdClass

def carregaTeste():
    lista = []
    for line in teste:
        fields = line.split(";")
        id = fields[0]
        classid = fields[2]
        lista.append(id)

        mapaTest[id] = classid

    return lista

def carregaRelacao(mapa, lista):
    
    acertei = 0
    errei = 0

    for l in lista:
        correta = mapaTest[l]
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
                    votes[classOther] = float(weight)
                else:
                    votes[classOther] += float(weight)
	
        max = 0
	escolhida = -1
	for (c,v) in votes.iteritems():
            if v > max:
                max = v
                escolhida = c

        if correta == escolhida:
            acertei += 1
        else:
            errei += 1

        print l, correta, escolhida, max, acertei, errei
        
    print "Avaliacao final =>>> acertei =  ", acertei , "  errei = " , errei


mapa = carregaTreino()
lista = carregaTeste()
carregaRelacao(mapa, lista)






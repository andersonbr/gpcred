
import sys

file = open(sys.argv[1])

interv = {"0-50": 0, "51-200" : 0, "200-600" : 0, "600-1000" : 0, "1000-3000":0 , "3000-10000": 0}

conta = {}
for l in interv:
    conta[l] = 0

for line in file:
    (classname, q) = line.split()
    qt = int(q)

    for l in interv:
        (li, ls) = l.split("-")

        if qt > int(li) and qt < int(ls):
            conta[l] += 1
            break

for k,v in conta.iteritems():
    print k.split("-")[0],v



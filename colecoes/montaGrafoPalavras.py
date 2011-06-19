import sys

file = open(sys.argv[1])

idTokens = { }

for line in file:

    tokens = line.strip(" \n;").split(";")
    id = tokens[0]
    year = tokens[1]
    classe = tokens[2]


    if id not in idTokens:
        idTokens[id] = []

#    print len(idTokens)

    for t in tokens[3::2]:
        if t not in idTokens[id]:
            idTokens[id].append(t)


while len(idTokens) > 0:
    (id, tokens) = idTokens.popitem()
        
    for (otherid, othertok) in idTokens.iteritems():
        comuns = 0
        for t in tokens:    
            if t in othertok:
                comuns+=1

        if comuns > 0:
            print id, otherid, comuns



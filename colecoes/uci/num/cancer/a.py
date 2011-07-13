
import sys

file = open(sys.argv[1])

id = 0
universal = 0
unico = {}

for line in file:
    
    if id == 0 :
        id+=1
        print line.strip()
        continue

    fields = line.strip().split(",")

    if fields[-1] == "2":
        classe = "a"
    else:
        classe = "b"

    for f in fields[1:-1]:
        
        #sys.stdout.write(str(f)+";1;")
        sys.stdout.write(f+",")
    
    print classe
#        print f, fields[f], key, v

    id+=1

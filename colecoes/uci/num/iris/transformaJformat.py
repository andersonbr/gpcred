
import sys

file = open(sys.argv[1])

id = 0
universal = 0
unico = {}

for line in file:

    fields = line.strip().split(",")
    sys.stdout.write(str(id)+ ";2011;"  +"CLASS=" + str(fields[-1]) + ";")
    fs = fields[0:-1]

    for f in fields[0:-1]:
        
        #sys.stdout.write(str(f)+";1;")
        sys.stdout.write(str(f)+";")

#        print f, fields[f], key, v

    print 
    id+=1

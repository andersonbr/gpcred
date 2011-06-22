import sys
import re

if len(sys.argv) <= 1:
	print "Please enter a filename as first argument."
	exit(1)


filename = sys.argv[1]

file = open(filename)


memory = {}
count = {}

for line in file:

    fields = line.split(";")
    classname = fields[2]
    id = fields[0]

#	print id, classname
   
    if classname not in memory:
		memory[classname] = [line.strip()]
    else:
		lista = memory[classname]
		lista.append(line.strip())
		memory[classname] = lista

    if classname not in count:
		count[classname] = 0

    count[classname] += 1;

linha = {0 : {}, 1 : {}, 2 : {} , 3 : {}}

for k,v in memory.iteritems():
    nums = k.split(".")
    
    stringcompleta = ""

    for i in range(0,4):
        stringcompleta += str(nums[i])
        
#        print stringcompleta
        
        if stringcompleta in linha[i]:
            linha[i][stringcompleta].append(v)
        else:
            linha[i][stringcompleta] = [v]
        
        stringcompleta += "."
        


for k,v in linha.iteritems():

    if k != 3:
        continue
#    print k
    
    for a,b in v.iteritems():
#        print a, len(b)
        for bl in b:
            for bll in bl:
                print re.sub("CLASS=\d+\.\d+\.\d+.\w+" , a ,bll)

#            if "ind" in line:
#                val = re.search("\d+\.\d+", line)
#                if val:
#                    val = float(val.group(0))
#                    print re.sub("\d+\.\d+",  str(100.0 - val), line),
#                else:
#                    print line,

#for k,v in count.iteritems():
# 	print k,"  ----->  " ,  v
# 	print k, v
#	

    
#print len(count), " classes" 

	

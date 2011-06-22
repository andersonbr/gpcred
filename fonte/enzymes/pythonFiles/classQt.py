import sys

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
		memory[classname] = [line]
	else:
		lista = memory[classname]
		lista.append(line)
		memory[classname] = lista
	
	if classname not in count:
		count[classname] = 0

	count[classname] += 1;

soma = 0

for k,v in count.iteritems():
# 	print k,"  ----->  " ,  v
 	print k, v
	

    
print len(count), " classes" 

	

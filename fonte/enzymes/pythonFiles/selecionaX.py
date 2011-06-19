import sys

if len(sys.argv) <= 2:
	print "Please enter a filename as first argument and a porcentage as second one."
	exit(1)


filename = sys.argv[1]
porcentage = sys.argv[2]

file = open(filename)


memory = {}
count = {}
newcount = {}

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

for k,v in count.iteritems():
#	print k,v	
	newcount[k] = int(int(v) * int(porcentage)/100)
	
for k,v in newcount.iteritems():
#	print k,v
	i = 0

	for l in memory[k]:
		if i < v:
        		sys.stdout.write(l)
		else:
			sys.stderr.write(l)
		i += 1
    

	

import sys
import random

if len(sys.argv) <= 2:
	print "Please enter a filename as first argument and a porcentage as second one."
	exit(1)


random.seed(1)

filename = sys.argv[1]
porcentage = int(sys.argv[2])

file = open(filename)

for line in file:
    r = random.randint(1,100)
#    print r
    if porcentage > r:
        print line,



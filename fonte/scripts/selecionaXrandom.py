import sys
import random

if len(sys.argv) <= 2:
	print "Please enter a filename and a percentage"
	exit(1)


filename = sys.argv[1]
percent = int(sys.argv[2])

file = open(filename)

for line in file:
    r = random.randint(0,100)

    if r < percent:
        print line,

    

import os
import sys

file = open(sys.argv[1])



def plotPopCommand(title, xlabel, ylabel,fileName = "_tmp"):
    
    sys.stdout = open("_to.plot",'w')
    s="""
        set terminal postscript landscape color "Helvetica" 19 
        set output '%s'
        #set style data boxes
        #set xtics (" " 1) 
        #set xtics 1
        set yrange[0:100]  
        #set xrange[:]  

        set xlabel "%s"
        set ylabel "%s"

        plot '%s' using ($1):($2) title "best" with linespoints, \
             '%s' using ($1):($3) title "avg" with linespoints, \
             '%s' using ($1):($4) title "worst" with linespoints

    """ % (title , xlabel, ylabel, fileName, fileName, fileName)
    print s
    sys.stdout = sys.__stdout__   
    


sys.stdout = open("_tmp",'w')
for line in file.readlines()[2:]: #from second line....
    line = line.strip()
    fields = line.split()
#    print fields
    print fields[0], 100.0 - float(fields[1]), 100.0 - float(fields[2]), 100.0 - float(fields[3])

sys.stdout = sys.__stdout__   

plotPopCommand("allpopulation.eps","Generations", "Fitness")

os.system('gnuplot _to.plot')
#os.system('rm -f _to.plot')
#os.system('rm -f _tmp')




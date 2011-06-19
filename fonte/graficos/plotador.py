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
        #set yrange[:]  
        #set xrange[:]  

        set xlabel "%s"
        set ylabel "%s"

        plot '%s' using ($1):($2) title "best" with lines 

    """ % (title , xlabel, ylabel, fileName)
    print s
    sys.stdout = sys.__stdout__   
    


sys.stdout = open("_tmp",'w')
for line in file.readlines()[2:]: #from second line....
    line = line.strip()
    fields = line.split()
#    print fields
    print fields[0], fields[1]

sys.stdout = sys.__stdout__   

plotPopCommand("allpopulation.eps","xlabel", "ylabel")

os.system('gnuplot _to.plot')
os.system('rm -f _to.plot')
os.system('rm -f _tmp')




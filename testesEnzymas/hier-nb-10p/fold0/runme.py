import sys
import time
import os

cmd = ""
t0 = time.time() 
for p in sys.argv[1:]:
    cmd += " " + p

os.system(cmd)
tf = time.time()

outfile = file("run.log","w")
outfile.write(cmd + '\n')
outfile.write("Time spent " + str(tf - t0) + " s\n")
outfile.close()


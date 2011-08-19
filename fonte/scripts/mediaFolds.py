
import sys
import math

def meanAndVar(values):
    
    means = {}
    deviates = {}
    

    for f in values:
    
        sum = 0
        for c in values[f]:
            sum += c
        
        mean = sum / len(values[f])
        means[f] = mean

    for f in values:
        sum2 = 0
        
        for c in values[f]:    
            sum2 += (c - means[f]) * (c - means[f])

        deviate = math.sqrt(sum2)
        deviates[f] = deviate

    sumpar = 0
    sumsquared = 0
    for v in range(0, len(values[3])):
        sumpar += (values[4][v] - values[3][v])
        sumsquared += ( (values[4][v] - values[3][v]) * (values[4][v] - values[3][v]))
    
#calculo de significancia estatistica
    t = 3.25  # critical value -> 9 degrees of freedom, 99% confidence (two-tailed)
    N = len(values[3])
    
    meanpar = sumpar / N
    stdDevpar = math.sqrt( (sumsquared - ((sumpar*sumpar)/N)) / N )
    offset = t * (stdDevpar / math.sqrt(N))
    l = meanpar - offset
    r = meanpar + offset

    
    print '%.4f, %.3f, %.2f, %.3f, %.2f'  %  ( 100 * means[1] , 100 * deviates[1] , 100 * means[2], 100 * deviates[2], (means[2]/means[1] - 1.0) * 100.0 )
    print "%.4f, %.3f, %.4f, %.3f, %.2f" % ( 100 * means[3] , 100 * deviates[3], 100 * means[4], 100 * deviates[4], (means[4]/means[3] - 1.0) * 100.0 )
    print "Test-t ( %.4f , %.4f ) " % (l , r )
    
    if l < 0 and r < 0:
        print "Statiscally worst"
    elif l <= 0 and r >= 0:
        print "Statistically equal"
    else:
        print "Statistically better"

 #   print (means[2] - means[1])


#    improvement = 


depotMicro = { 1 : [] , 2 : [] , 3 : [] , 4 : [] }
depotMacro = { 1 : [] , 2 : [] , 3 : [] , 4 : [] }
#depotMicro = { "TrainBaseline" : [] , "TrainBest" : [] , "TestBaseline" : [] , "TestBest" : [] }
#depotMacro = { "TrainBaseline" : [] , "TrainBest" : [] , "TestBaseline" : [] , "TestBest" : [] }

for p in range(1, len(sys.argv)):
    f = open(sys.argv[p])

    counter = 1
    for line in f:
        fields = line.strip().split()
#        print fields

        depotMicro[counter].append(float(fields[1]))
        depotMacro[counter].append(float(fields[2]))
        counter += 1

print "Micro Results"
meanAndVar(depotMicro)

print "\nMacro Results"
meanAndVar(depotMacro)




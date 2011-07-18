
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

#    for v in values[1]:
#        sumpar += (values[2] - values[1])
#        sumsquared += ( (values[2] - values[1]) * (values[2] - values[1]))
    
    #calculo de significancia estatistica
#    t = 3.25  # critical value -> 9 degrees of freedom, 99% confidence (two-tailed)
#    N = len(values[1])
#    meanpar = sumpar / N
#    stdDevpar = math.sqrt( (sumsquared - ((sumpar*sumpar)/N)) / N )
#    offset = t * (stdDevpar / sqrt(N))
#    l = mean - offset
#    r = mean + offset

#    for f in range(3,4):
        
    print 'Baseline Deviate Best Deviate Gain'
    print 'Train %.2f, %.3f, %.2f, %.3f, %.2f'  %  ( means[1] , deviates[1] , means[2], deviates[2], (means[2]/means[1] - 1.0) * 100.0 )
    print "Test %.2f, %.3f, %.2f, %.3f, %.2f" % ( means[3] , deviates[3], means[4], deviates[4], (means[4]/means[3] - 1.0) * 100.0 )
    
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

meanAndVar(depotMicro)
meanAndVar(depotMacro)




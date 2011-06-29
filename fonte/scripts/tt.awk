BEGIN {
    sum=0;
    sumSquared =0;
    N=0;
    t=3.25; # critical value -> 9 degrees of freedom, 99% confidence (two-tailed)

        sumC=0;
    sumSquaredC=0;
    sumB=0;
    sumSquaredB=0;
}

{
    sum += ($3 - $2);
    sumSquared += (($3 - $2) * ($3 - $2));
    N++;

    sumC += $3;
    sumSquaredC += (($3) * ($3));

    sumB += ($2);
    sumSquaredB += (($2) * ($2));
}

END {
    mean=sum/N;
    stdDev = sqrt( (sumSquared - ((sum*sum) / N)) / N);
    offset= t * (stdDev / sqrt(N));
    l=mean - offset;
    u=mean + offset;

    meanC=sumC/N;
    stdDevC = sqrt( (sumSquaredC - ((sumC*sumC) / N)) / N);

    meanB=sumB/N;
    stdDevB = sqrt( (sumSquaredB - ((sumB*sumB) / N)) / N);

    gain=((meanC/meanB)-1)*100;
    print "Baseline: "meanB*100" += "stdDevB*100;
    print "TermCred: "meanC*100" += "stdDevC*100;
    print "GAIN: "gain"%";
    print "T-TEST: ( "l", "u" )";
}


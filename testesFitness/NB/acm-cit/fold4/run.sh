fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')
base=acm

path=../../../../arquivos/$base

treinopath=$path/CV5/treino/treino$fold
validacaopath=$path/CV5/validacao/validacao$fold
testepath=$path/CV5/teste/teste$fold
brunoro=fold$fold.gpvis

graphpath="../../../../fonte/graphs/"

grafosaut="-g 1 $graphpath/authorship.txt"
grafoscit="-g 1 $graphpath/citation.txt"
grafosboth="-g 2 $graphpath/authorship.txt $graphpath/citation.txt"



python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -dontsave $grafoscit -noterms



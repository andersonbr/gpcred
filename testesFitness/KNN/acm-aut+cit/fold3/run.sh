fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

base=acm

path=../../../../arquivos/$base

treinopath=$path/CV5/treino/treino$fold
validacaopath=$path/CV5/validacao/validacao$fold
testepath=$path/CV5/teste/teste$fold
brunoro=fold$fold.gpvis

k=16
grafoaut="-g 1  ../../../../fonte/graphs/authorship.txt"
grafocit="-g 1  ../../../../fonte/graphs/citation.txt"
grafoboth="-g 2  ../../../../fonte/graphs/authorship.txt ../../../../fonte/graphs/citation.txt"

fitness=""

python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -dontsave -knn $k $grafoboth -noterms



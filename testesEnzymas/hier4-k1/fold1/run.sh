fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

path=../../../fonte/enzymes/files/hier4/

treinopath=$path/fold$fold/_treino$fold
validacaopath=$path/fold$fold/_validacao$fold
testepath=$path/fold$fold/_teste$fold
brunoro=fold$fold.gpvis

graph=../../../fonte/enzymes/top950.blast.ok3.sorted

python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -num 15 -noterms -knn 1 -KNNOptimize -g 1 $graph



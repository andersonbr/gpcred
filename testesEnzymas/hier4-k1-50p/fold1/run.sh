fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

base=$(pwd | sed 's/\/fold/ /g' | cut -f 1 -d' ' | sed 's/\NB\// /g' | cut -f 2 -d' ')

path=../../../fonte/enzymes/files/hier4/

#../../../fonte/enzymes/files/hier4/fold0/

treinopath=$path/fold$fold/_treino$fold
validacaopath=$path/fold$fold/_validacao$fold
testepath=$path/fold$fold/_teste$fold
brunoro=fold$fold.gpvis

grafopath=$path/../../top950.blast.ok3.sorted

python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -noterms -g 1 $grafopath -num 15 -knn 1 -knnoptimize 



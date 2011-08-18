fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

path=../../../../fonte/enzymes/files/hier4/

treinopath=$path/fold$fold/_treino$fold
validacaopath=$path/fold$fold/_validacao$fold
testepath=$path/fold$fold/_teste$fold
brunoro=fold$fold.gpvis

grafopath=$path/../../top950.blast.ok3.sorted

python knnEmGrafo.py $treinopath $validacaopath $grafopath $testepath



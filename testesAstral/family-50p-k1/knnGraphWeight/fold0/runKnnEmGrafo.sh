fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

path=../../../../fonte/enzymes/astral/family50p/

treinopath=$path/treino/_treino$fold
validacaopath=$path/validacao/_validacao$fold
testepath=$path/teste/_teste$fold
brunoro=fold$fold.gpvis

grafopath=$path/outfile.astral.blast.ok4 

python knnEmGrafo.py $treinopath $validacaopath $grafopath $testepath



fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

path=../../../../fonte/enzymes/astral/family10f/fold$fold

treinopath=$path/_treino
validacaopath=$path/_validacao
testepath=$path/_teste
brunoro=fold$fold.gpvis

grafopath=$path/../../family10p/outfile.astral.blast.ok4 

python knnEmGrafo.py $treinopath $validacaopath $grafopath $testepath



fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

base=../../../fonte/enzymes/astral/family50p
treinopath=$base/treino/_treino$fold
validacaopath=$base/validacao/_validacao$fold
testepath=$base/teste/_teste$fold

k=1
brunoro=fold$fold.gpvis

graph=$base/outfile.astral.blast.ok4

python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -knn $k -num 15 -g 1 $graph

#echo "python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -knn $k -num 15 -g 1 $graph"


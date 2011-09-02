fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

base=../../../fonte/enzymes/astral/family10f
treinopath=$base/fold$fold/_treino
validacaopath=$base/fold$fold/_validacao
testepath=$base/fold$fold/_teste

k=1
brunoro=fold$fold.gpvis

graph=$base/../outfile.astral.blast.ok4

#python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -knn $k -num 15 -g 1 $graph -load last.pop
echo "python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -knn $k -num 15 -g 1 $graph -load last.pop"


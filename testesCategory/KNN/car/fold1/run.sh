cat=$(cat ../collums.number); 
fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')
colecao=$(pwd | sed 's/\/fold/ /g' | cut -f 1 -d' '| sed 's/\/KNN\// /g' | cut -f 2 -d' ')

base=../../../../colecoes/uci/cat/$colecao/fold$fold/
k=9

treinopath=$base/_treino$fold.jformat
validacaopath=$base/_validacao$fold.jformat
testepath=$base/_teste$fold.jformat
brunoro=fold$fold.gpvis


python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -dontsave -cat $cat -knn $k



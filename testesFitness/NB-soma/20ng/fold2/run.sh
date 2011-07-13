fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')

base=$(pwd | sed 's/\/fold/ /g' | cut -f 1 -d' ' | sed 's/\NB-soma\// /g' | cut -f 2 -d' ')
echo $base
echo $fold

path=../../../../arquivos/$base

treinopath=$path/CV5/treino/treino$fold
validacaopath=$path/CV5/validacao/validacao$fold
testepath=$path/CV5/teste/teste$fold
brunoro=fold$fold.gpvis

macro="-sumMacro"

python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -dontsave $macro



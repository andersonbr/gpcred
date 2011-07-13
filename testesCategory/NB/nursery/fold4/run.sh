cat=$(cat ../collums.number); 
fold=4
colecao=nursery

base=../../../../colecoes/uci/cat/$colecao/fold$fold/

treinopath=$base/_treino$fold.jformat
validacaopath=$base/_validacao$fold.jformat
testepath=$base/_teste$fold.jformat
brunoro=fold$fold.gpvis


python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -dontsave -cat $cat



cat=$(cat ../collums.number); 
fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')
colecao=$(pwd | sed 's/\/fold/ /g' | cut -f 1 -d' '| sed 's/\/KNN\// /g' | cut -f 2 -d' ')

base=../../../../colecoes/uci/cat/$colecao/fold$fold/

treinopath=$base/_treino$fold.jformat
validacaopath=$base/_validacao$fold.jformat
testepath=$base/_teste$fold.jformat
brunoro=fold$fold.gpvis


max=0
maxk=0

for k in {1..20}; do 

    python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -brunoro $brunoro -dontsave -cat $cat -evalonly -knn $k > _temp 2>&1
    m=$( cat _temp | grep MicroF1 | head -n1 | cut -f 2 -d' ' | awk '{ print ($1*10000)}' | cut -f 1 -d'.')
    rm _temp

    echo "$k --->>> $m" 

    if [ $m -gt $max ]; then
        max=$m
        maxk=$k
    fi

done

echo $maxk
echo $max


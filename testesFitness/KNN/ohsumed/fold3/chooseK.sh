fold=$(pwd | sed 's/fold/ /g' | cut -f2 -d' ')
base=ohsumed

path=../../../../arquivos/$base

treinopath=$path/CV5/treino/treino$fold
validacaopath=$path/CV5/validacao/validacao$fold
testepath=$path/CV5/teste/teste$fold
brunoro=fold$fold.gpvis

max=0
maxk=0

for k in {1..20}; do 

    python runme.py ./gp-cred -treino $treinopath -validacao $validacaopath -teste $testepath -evalonly -knn $k > _temp 2>&1
    m=$( cat _temp | grep MicroF1 | head -n1 | cut -f 2 -d' ' | awk '{ print ($1*10000)}' | cut -f 1 -d'.')
#    rm _temp

    echo "$k --->>> $m" 

    if [ $m -gt $max ]; then
        max=$m
        maxk=$k
    fi

done

echo $maxk
echo $max


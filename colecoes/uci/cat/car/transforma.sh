
file=$1

python ../../../../fonte/scripts/treinoValTeste.py $file 60 20

numCol=$( cat $file | sed "s/,/ /g" | awk 'END{print NF}')

let numCol=numCol-1

for i in $(seq 1 $numCol); do echo -n "$i," >> _cab; done ; echo "class" >> _cab

for a in treino validacao teste; do python transformaJformat.py _$a > _${a}.jformat ; done
for a in treino validacao teste; do mv _$a _${a}.csv ; done

for a in treino validacao teste; do cat _cab _${a}.csv > _a ; mv _a _${a}.csv ; done


for a in treino validacao teste; do java -classpath CLASSPATH:/home/palotti/weka/weka-3-6-4/weka.jar weka.core.converters.CSVLoader _$a.csv > _${a}.arff ; done

rm _cab



file=$1
python ../../../../fonte/scripts/cv.py $file
for a in treino teste validacao ; do for f in 0 1 2 3 4 ; do python transformaJformat.py _${a}${f} > fold$f/_${a}${f}.jformat ; done ; done

for a in treino teste validacao ; do for f in 0 1 2 3 4 ; do rm _${a}${f} ; done ; done



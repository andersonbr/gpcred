
for f in 3 4; do


cd fold${f}

time python best.py

cd -

done

python mediaCat.py fold*/eval.out




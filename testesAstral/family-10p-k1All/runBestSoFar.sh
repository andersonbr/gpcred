
for f in 0 1 2 3 4; do

cp fold${f}/cred.best fold${f}_/.

cd fold${f}_

python best.py

cd -

done

python mediaCat.py fold*_/eval.out




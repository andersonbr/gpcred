
cd fold0
python best.py

cd ../fold1
python best.py

cd ../fold2
python best.py

cd ../fold3
python best.py

cd ../fold4
python best.py

cd ..


python mediaCat.py fold*/eval.out


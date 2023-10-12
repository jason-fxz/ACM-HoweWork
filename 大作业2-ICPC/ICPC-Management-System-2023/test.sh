g++ main.cpp -o main -O2
time -p ./main < ./data/1.in > out
diff out ./data/1.out -Z
time -p ./main < ./data/2.in > out
diff out ./data/2.out -Z
time -p ./main < ./data/3.in > out
diff out ./data/3.out -Z
time -p ./main < ./data/4.in > out
diff out ./data/4.out -Z
time -p ./main < ./data/5.in > out
diff out ./data/5.out -Z
time -p ./main < ./data/6.in > out
diff out ./data/6.out -Z
time -p ./main < ./data/7.in > out
diff out ./data/7.out -Z
time -p ./main < ./data/big.in > out
diff out ./data/big.out -Z
time -p ./main < ./data/bigger.in > out
diff out ./data/bigger.out -Z
time -p ./main < ./data/error.in > out
diff out ./data/error.out -Z
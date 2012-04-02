#! /bin/bash -e

./main train1.csv test1.csv 0 > prediction.txt
./main train2.csv test2.csv 1 >> prediction.txt

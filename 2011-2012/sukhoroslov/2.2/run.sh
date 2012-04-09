#!/bin/bash

function test
{
    ./gen $1 $2 $3 $4 > input.txt
    mpirun -np $5 multi_life > multi.out
    cat input.txt | ./single_life > single.out
    diff multi.out single.out
}

test 10 20 3 0.5 3
test 30 3 4 0.3 3
test 40 10 10 0.7 4
test 100 100 100 0.6 4



#!/bin/sh

function test()
{
    echo "test $3"
    n=$1
    m=$2
    `./gen $n $m > input`
#    cat input
    outputNaive=`cat input | ./naive`
    outputSolution=`cat input | ./solution`
    if [ "$outputNaive" = "$outputSolution" ]; then
        echo "OK"
    else
        echo "WA: $outputNaive $outputSolution"
    fi
}

test 10 10 1
test 9 10 2
test 10 9 3
test 8 5 4
test 4 9 5
test 3 4 6
test 1 10 7
test 10 1 8
test 5 5 9
test 1 1 10

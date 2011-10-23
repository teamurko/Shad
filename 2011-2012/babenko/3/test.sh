#!/bin/sh

for i in 1 2 3 4 5 6 7 8 999 100 104 153 173 183 10000 100000 1000000 99999; do
    `./gen $i $i > input`
    output_naive=`./naive < input`
    output_solution=`./solution < input`
    if [ "$output_naive" = "$output_solution" ]; then
        echo "OK"
    else
        echo "WA: $output_naive vs $output_solution"
    fi
done

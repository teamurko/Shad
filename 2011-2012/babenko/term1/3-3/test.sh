for i in 10 20 30 40 50 60 100 1000 10000 100000 1000000 999699 999499 999399 992999 999999 12324 234235 454353 234356 999999 676767; do
#for i in 10; do
    m=$i
    let "n=$m / 10"
    if [ $n -gt 100000 ]; then
        n=100000
    fi
    echo $n " " $m
    ./gen $n $m > input.txt
#    cat input.txt | ./stupid > stupid.out
    cat input.txt | ./solution > solution.out
#    diff stupid.out solution.out
done

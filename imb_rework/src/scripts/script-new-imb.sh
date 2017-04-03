#!/bin/bash

for R in 100 1000 10000; do
    if [ "$R" == 100 ]; then W=20; fi
    if [ "$R" == 1000 ]; then W=200; fi
    if [ "$R" == 10000 ]; then W=2000; fi
    for SIZE in 2 16 256 2048 32768 262144 2097152; do 
        for N in 4 8 16; do 
            for i in `seq 0 1 100`; do 
                rm -f out.?; 
                mpirun -np $N -outfile-pattern=out.%r ./imb bcastmt -count $SIZE -warmup $W -repeat $R; 
                cat out.0 | /bin/grep 'pattern: ' | awk '{print $6}'>> avg.$SIZE.$N.txt; 
            done; 
        done; 
    done; 
    mkdir -p r$R
    mv *.*.txt r$R; 
    tar czf ~/txt-new-imb-r$R.tgz r$R
done


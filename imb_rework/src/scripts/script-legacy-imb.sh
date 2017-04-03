#!/bin/bash

for R in 100 1000 10000; do
    for SIZE in 2 16 256 2048 32768 262144 2097152; do 
        echo $SIZE > msglens;
        for N in 4 8 16; do 
            for i in `seq 0 1 100`; do 
                rm -f out.?; 
#                mpirun -np $N -outfile-pattern=out.%r ./imb bcastmt -count $SIZE -warmup $W -repeat $R; 
                mpirun -np $N -outfile-pattern=out.%r ./IMB-MPI1 -npmin $N -msglen msglens -iter_policy off -iter $R bcast
#                cat out.0 | /bin/grep '>> avg' | awk '{print $3}'>> avg.$SIZE.$N.txt; 
                cat out.0 | /bin/grep "$SIZE[ ]*$R" | awk '{print $5}'>> avg.$SIZE.$N.txt;
            done; 
        done; 
    done; 
    mkdir -p r$R
    mv *.*.txt r$R; 
    tar czf ~/txt-legacy-imb-r$R.tgz r$R
done


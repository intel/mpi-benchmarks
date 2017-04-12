for N in 4 8 16; do
        for R in 100 1000 10000; do
                for SIZE in 2 16 256 2048 32768 262144 2097152; do
                        for i in r$R/avg.$SIZE.$N.txt; do
                                cat $i | awk -v SIZE=$SIZE 'BEGIN{MIN=MIN2=100000;MAX=MAX2=-100000;PREV=-1} { if (PREV>0 && $1>PREV*2) next; if ($1<MIN) MIN=$1; else if ($1<MIN2) MIN2=$1; if($1>MAX) MAX=$1; else if ($1>MAX2) MAX2=$1; SUM+=$1; SQSUM+=$1*$1; N++; PREV=$1 } END { M=SUM/N; D=(SQSUM-SUM*SUM/N)/(N-1); s=sqrt(D); print SIZE " " (MAX-MIN)/M*100 " " (MAX2-MIN2)/M*100 " " s/M*100 }';
                        done
                done > plt.$R.$N.txt
    done
done

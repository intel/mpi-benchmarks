for N in 4 8 16; do for SIZE in 2 16 256 2048 32768 262144 2097152; do for i in r10*/avg.$SIZE.$N.txt; do cat $i | awk 'BEGIN{MIN=100000;MAX=-100000;PREV=-1} { if (PREV>0 && $1>PREV*2) next; if ($1<MIN) MIN=$1;
 if($1>MAX) MAX=$1; SUM+=$1; SQSUM+=$1*$1; N++; PREV=$1 } END { M=SUM/N; D=(SQSUM-SUM*SUM/N)/(N-1); s=sqrt(D); print (MAX-MIN)/M*100 " " s/M*100 }'; done > avg.$SIZE.$N.txt; done; done

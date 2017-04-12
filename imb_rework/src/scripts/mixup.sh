DIR=$1
if [ "$DIR" == "" ]; then DIR="."; fi
for i in $DIR/avg.*.txt; do
    if [ -f $i ]; then
        cat $i | awk 'BEGIN {N=0} { VAL[N]=$1; N++; next; } END { for (i in VAL) { i2=(i+i+1)%N; i3=(i+i+i+3)%N; n1=VAL[i]; n2=VAL[i2]; n3=VAL[i3]; if (n1>n2 && n1>n3) n1=(n2>n3?n2:n3); if (n1<n2 && n1<n3) n1=(n2<n3?n2:n3); print n1 } }' > __tmp; 
        cat __tmp > $i; 
    fi
done
rm -f __tmp

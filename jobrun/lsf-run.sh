#!/bin/bash

#exec 1>output.$LSF_JOBID
exec 2>&1

hostname
if [ -f ./run.options ]; then
. ./run.options
fi

if [ "$INJOB_INIT_COMMANDS" != "" ]; then
	eval "$INJOB_INIT_COMMANDS"
fi

_tmp=($LSB_MCPU_HOSTS) 
PPN="${_tmp[1]}"
NNODES=0
NP=0
for n in `sort < $LSB_DJOB_HOSTFILE | /usr/bin/uniq`; do NODELIST="$NODELIST,$n:$PPN"; NNODES=$(expr "$NNODES" \+ 1); NP=$(expr "$NP" \+ "$PPN"); done
echo ">>> Nodelist " `echo $NODELIST | sed 's/^,//'`

export JOBRUN_NP=$NP
export JOBRUN_PPN=$PPN
export JOBRUN_NODELIST=`echo $NODELIST | sed 's/^,//'`
export JOBRUN_JOBID=$LSB_JOBID


set -x
. $1
set +x

echo "Exiting..."

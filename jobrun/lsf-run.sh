#!/bin/bash

#exec 1>output.$LSF_JOBID
exec 2>&1

hostname
if [ -f "$1" ]; then
. "$1"
fi

shift

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
. "$RUN_SH"
set +x

echo "Exiting..."

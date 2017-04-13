#!/bin/bash

#exec 1>output.$LSF_JOBID
exec 2>&1

if [ -f ./run.options ]; then
. ./run.options
fi

if [ "$INJOB_INIT_COMMANDS" != "" ]; then
	eval "$INJOB_INIT_COMMANDS"
fi

NNODES=$SLURM_NNODES
NP=$SLURM_NPROCS
PPN=$($SLURM_NPROCS / $SLURM_NNODES)

#PPN="`echo $SLURM_JOB_CPUS_PER_NODE | sed 's/(x[0-9]\+)$//'`"

NODELIST=""
RES=`expr index $SLURM_JOB_NODELIST "[,"`
if [ $RES -ne 0 ]; then
    for e in `echo $SLURM_JOB_NODELIST | sed 's/,\([^0-9]\)/ \1/g'`; do
        RES=`expr index $e "["`
        if [ $RES -ne 0 ]; then
            NBASE="`echo $e | sed 's/^\(.*\)\[.*$/\1/'`"
            NLIST="`echo $e | sed 's/^.*\[\(.\+\)\]$/\1/;s/,/ /g'`"
            for i in $NLIST; do
                RES=`expr index $i "-"`
                if [ $RES -ne 0 ]; then
                    x="`echo $i | cut -d- -f1`"
                    y="`echo $i | cut -d- -f2`"
                    for n in `seq -w $x $y`; do
                        [ -z "$NODELIST" ] || NODELIST=$NODELIST","
                        NODELIST=$NODELIST$NBASE$n:$PPN
                    done
                else
                    [ -z "$NODELIST" ] || NODELIST=$NODELIST","
                    NODELIST=$NODELIST$NBASE$i:$PPN
                fi
            done
        else
            [ -z "$NODELIST" ] || NODELIST=$NODELIST","
            NODELIST=$NODELIST$e:$PPN
        fi
    done
else
        NODELIST=$SLURM_JOB_NODELIST:$PPN
fi

#NP=$(expr $NNODES \* $PPN)

#for n in `sort < $ | /usr/bin/uniq`; do NODELIST="$NODELIST,$n:$PPN"; NNODES=$(expr "$NNODES" \+ 1); NP=$(expr "$NP" \+ "$PPN"); done
echo ">>> Nodelist " `echo $NODELIST | sed 's/^,//'`

export JOBRUN_NP=$NP
export JOBRUN_PPN=$PPN
export JOBRUN_NODELIST=`echo $NODELIST | sed 's/^,//'`
export JOBRUN_JOBID=$SLURM_JOBID


set -x
. $1
set +x

echo "Exiting..."

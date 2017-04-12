#!/bin/bash

cd $PBS_O_WORKDIR

exec 1>output.$PBS_JOBID
exec 2>&1

hostname
if [ -f ./run.options ]; then
. ./run.options
fi

if [ "$INJOB_INIT_COMMANDS" != "" ]; then
	eval "$INJOB_INIT_COMMANDS"
fi

for n in `sort < $PBS_NODEFILE | uniq`; do NODELIST="$NODELIST,$n:$PBS_NUM_PPN"; done
echo ">>> Nodelist " `echo $NODELIST | sed 's/^,//'`

export JOBRUN_NP=$PBS_NP
export JOBRUN_PPN=$PBS_NUM_PPN
export JOBRUN_NODELIST=`echo $NODELIST | sed 's/^,//'`
export JOBRUN_JOBID=`echo $PBS_JOBID | awk -F. '{print $1}'`

set -x
. $1
set +x

echo "Exiting..."

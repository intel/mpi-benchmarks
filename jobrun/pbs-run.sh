#!/bin/bash

cd $PBS_O_WORKDIR

export JOBRUN_JOBID=`echo $PBS_JOBID | awk -F. '{print $1}'`
export JOBRUN_NP=$PBS_NP
export JOBRUN_PPN=$PBS_NUM_PPN
export JOBRUN_DIRNAME=$(cd $(dirname "$0") && pwd -P)

exec 1>output.$JOBRUN_JOBID
exec 2>&1

if [ -f "$1" ]; then
. "$1"
fi

shift

if [ -z "$RUN_SH" ]; then RUN_SH="./run.sh"; fi
if [ -z "$BINARY_TO_RUN" ]; then BINARY_TO_RUN="hostname"; fi

if [ "$INJOB_INIT_COMMANDS" != "" ]; then
	eval "$INJOB_INIT_COMMANDS"
fi

for n in `sort < $PBS_NODEFILE | uniq`; do NODELIST="$NODELIST,$n:$PBS_NUM_PPN"; done
echo ">>> Nodelist " `echo $NODELIST | sed 's/^,//'`

export JOBRUN_NODELIST=`echo $NODELIST | sed 's/^,//'`

set -x
. "$RUN_SH"
set +x

echo "Exiting..."

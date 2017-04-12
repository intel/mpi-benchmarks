env
mpiexec.hydra -np "$JOBRUN_NP" --errfile-pattern=err.$JOBRUN_JOBID --outfile-pattern=out.$JOBRUN_JOBID.%r "$2" $3
sleep 5

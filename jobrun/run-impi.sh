env
rm -f err.$JOBRUN_JOBID.* out.$JOBRUN_JOBID.*
mpiexec.hydra -np "$JOBRUN_NP" --errfile-pattern=err.$JOBRUN_JOBID.%r --outfile-pattern=out.$JOBRUN_JOBID.%r "$2" $3
sleep 5

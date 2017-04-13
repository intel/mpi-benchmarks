jobctl_check_job_status() {
    qstr=`qstat $jobid_short 2>&1 | grep "^$jobid_short"`
    if [ -z "$qstr" -a ! -f "$FILE_OUT" ]; then
        echo "JOB DISAPPEARED!"
        jobstatus="NONE"
        return
    fi
    jobstatus=`echo $qstr | awk '{print $5}'`
    if [ "$jobstatus" == "" ]; then
	    echo "JOB DISAPPEARED!"
  	    jobstatus="NONE"
  	    return
    fi
    if [ "$jobstatus" != "$oldjobstatus" ]; then echo "Job status: $jobstatus"; fi
    oldjobstatus=$jobstatus
    if [ "$jobstatus" != "R" -a "$jobstatus" != "C" -a "$jobstatus" != "E" -a "$jobstatus" != "Q" ]; then
	    echo ">> qstat out: $qstr"
	    jobstatus="NONE"
    fi
}

jobctl_check_job_done() {
    cat "$FILE_OUT" | awk -v NNODES=1 '/Exiting.../ { done++; if (done == NNODES) { print "done"; exit 1; } } { next; }'
    [ "$?" == 1 ] && jobdone=1
}

jobctl_cancel() {
	if [ "$jobid" != "" -a "$jobcancelled" == "" ]; then
        qdel $jobid
		jobcancelled="$jobid"
	fi
}

jobctl_submit() {
    OUTFILE=`mktemp -u .out.XXXXXXX`
    rm -f "$OUTFILE"

    PBS_FILE=`mktemp -u submit.XXXXXXX.pbs`

    echo "$JOBRUN_DIRNAME/pbs-run.sh" \"$RUN_SH\" \"$BINARY_TO_RUN\" \"$ARGS\" >> $PBS_FILE
    if [ "$NODETYPE" != "" ]; then
        WITH_NODETYPE=":$NODETYPE"
    fi
	qsub -l "nodes=$NNODES:ppn=$PPN$WITH_NODETYPE,walltime=00:$TIME_LIMIT:00" -q "$QUEUE" "$PBS_FILE" 2>&1 | tee "$OUTFILE"
    
    grep -q "^[0-9]*\.$QUEUE_SUFFIX$" "$OUTFILE"
    if [ $? != "0" ]; then rm -f "$OUTFILE"; exit 0; fi

    jobid=`grep "^[0-9]*\.$QUEUE_SUFFIX$" "$OUTFILE"`
    export jobid
    jobid_short=`echo $jobid | awk -F. '{ print $1 }'`
    export jobid_short
    sleep 2
}

jobctl_set_paths() {
    SCRATCH_PWD="$PWD"
    SCRATCH_HOME="$HOME"
}

jobctl_set_outfiles() {
    FILE_OUT=output.$jobid_short
}

jobctl_move_outfiles() {
    if [ -f "$FILE_OUT" -a "$jobid" != "" ]; then
    mkdir -p $SCRATCH_PWD/results.$jobid_short
    mv $FILE_OUT $SCRATCH_PWD/results.$jobid_short
    mv $PBS_FILE $SCRATCH_PWD/results.$jobid_short
    RES=`ls -1 $SCRATCH_PWD/out.$jobid_short* 2> /dev/null`
    if [ ! -z "$RES" ]; then
            mv $SCRATCH_PWD/out.$jobid_short* $SCRATCH_PWD/results.$jobid_short
    fi
    RES=`ls -1 $SCRATCH_PWD/err.$jobid_short* 2> /dev/null`
    if [ ! -z "$RES" ]; then
            mv $SCRATCH_PWD/err.$jobid_short* $SCRATCH_PWD/results.$jobid_short
    fi
    RES=`ls -1 $SCRATCH_HOME/stack.* 2> /dev/null`
    if [ ! -z "$RES" ]; then
        mv  $SCRATCH_HOME/stack.* $SCRATCH_PWD/results.$jobid
    fi
    rm -f __old.out __tmp __nodelist __benchmark __diff.out
    rm -f $PBS_FILE.o* $PBS_FILE.e*
    echo "Results collected:" $SCRATCH_PWD/results.$jobid_short
    fi
}

jobctl_cleanup() {
	echo "CLEANUP..."
	jobctl_cancel
	sleep 1
	jobctl_move_outfiles
	rm -f "$OUTFILE" 
	echo "CLEANUP DONE"
	exit 1
}

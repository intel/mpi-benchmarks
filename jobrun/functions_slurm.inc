jobctl_check_job_status() {
    qstr=`squeue -p "$QUEUE" 2>&1 | grep "$jobid_short"`
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
    if [ "$jobstatus" != "R" -a "$jobstatus" != "PD" -a "$jobstatus" != "CG" ]; then
        echo ">> squeue out: $qstr"
        jobstatus="NONE"
    fi
    if [ "$jobstatus" == "CG" ]; then
	    jobstatus="E"
    fi
    if [ "$jobstatus" == "PD" ]; then
	    jobstatus="Q"
    fi
}

jobctl_check_job_done() {
        cat "$FILE_OUT" | awk -v NNODES=1 '/Exiting.../ { done++; if (done == NNODES) { print "done"; exit 1; } } { next; }'
        [ "$?" == 1 ] && jobdone=1
}

jobctl_cancel() {
	if [ "$jobid" != "" -a "$jobcancelled" == "" ]; then
        	scancel $jobid
		jobcancelled="$jobid"
	fi
}

jobctl_submit() {
    OUTFILE=`mktemp -u .out.XXXXXXX`
    rm -f "$OUTFILE"

    n=$(expr "$NNODES" \* "$PPN")
    sbatch --time=${TIME_LIMIT} -D "$PWD" -N "$NNODES" -n "$n" -p "$QUEUE" "slurm-run.sh" "$OPTSFILE" "$ARGS" 2>&1 | tee "$OUTFILE"
    grep "Batch job submission failed" "$OUTFILE" && exit 0

    grep -q "Submitted batch job " "$OUTFILE"
    if [ $? != "0" ]; then rm -f "$OUTFILE"; exit 0; fi

    qstr=`grep "Submitted batch job " "$OUTFILE"`
    jobid=`echo $qstr | cut -d ' ' -f 4`
    jobid_short=$jobid
    rm -f "$OUTFILE"
}

jobctl_set_paths() {
    SCRATCH_PWD=`echo $PWD | sed "s!$HOME!$HOME/_scratch/!"`
    SCRATCH_HOME="$HOME/_scratch"
}

jobctl_set_outfiles() {
    FILE_OUT=$SCRATCH_PWD/slurm-$jobid.out
}

jobctl_move_outfiles() {
    if [ -f "$FILE_OUT" -a "$jobid" != "" ]; then
        mkdir -p $SCRATCH_PWD/results.$jobid_short
        mv $FILE_OUT $SCRATCH_PWD/results.$jobid_short
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

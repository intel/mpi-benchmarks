jobctl_check_job_status() {
    qstr=`bjobs $jobid_short 2>&1 | grep "^$jobid_short"`
    if [ -z "$qstr" -a ! -f "$FILE_OUT" ]; then
        echo "JOB DISAPPEARED!"
        jobstatus="NONE"
        return
    fi
    jobstatus=`echo $qstr | awk '{print $3}'`
    if [ "$jobstatus" == "" ]; then
	    echo "JOB DISAPPEARED!"
  	    jobstatus="NONE"
  	    return
    fi
    if [ "$jobstatus" != "$oldjobstatus" ]; then echo "Job status: $jobstatus"; fi
    oldjobstatus=$jobstatus
    if [ "$jobstatus" != "RUN" -a "$jobstatus" != "PEND" -a "$jobstatus" != "EXIT" -a "$jobstatus" != "DONE" ]; then
	    echo ">> qstat out: $qstr"
	    jobstatus="NONE"
    fi
    if [ "$jobstatus" == "RUN" ]; then
	jobstatus="R"
    fi
    if [ "$jobstatus" == "PEND" ]; then
	jobstatus="P"
    fi
    if [ "$jobstatus" == "EXIT" ]; then
	jobstatus="C"
    fi
    if [ "$jobstatus" == "DONE" ]; then
	jobstatus="C"
    fi
}

jobctl_check_job_done() {
        cat "$FILE_OUT" | awk -v NNODES=1 '/Exiting.../ { done++; if (done == NNODES) { print "done"; exit 1; } } { next; }'
        [ "$?" == 1 ] && jobdone=1
}

jobctl_cancel() {
	if [ "$jobid" != "" -a "$jobcancelled" == "" ]; then
        	bkill $jobid
		jobcancelled="$jobid"
	fi
}

jobctl_submit() {
    OUTFILE=`mktemp -u .out.XXXXXXX`
    rm -f "$OUTFILE"

    LSF_FILE=`mktemp -u submit.XXXXXXX.lsf`

    echo "lsf-run.sh" \"$OPTSFILE\" \"$ARGS\" >> $LSF_FILE
    if [ "$NODETYPE" != "" ]; then
        WITH_NODETYPE="select[$NODETYPE]"
    fi
    n=$(expr "$NNODES" \* "$PPN")
    bsub -q "$QUEUE" -l NMIWATCHDOG_OFF=1 -n $n -R "{$WITH_NODETYPE span[ptile=$PPN]}" -W 00:$TIME_LIMIT -o output.%J "$PWD/lsf-run.sh" "$RUN_SH" "$BINARY_TO_RUN" $ARGS 2>&1 > "$OUTFILE"
   
    grep -q "^Job <[0-9]*> is submitted to queue <" "$OUTFILE"
    if [ $? != "0" ]; then rm -f "$OUTFILE"; exit 0; fi

    qstr=`grep "^Job <[0-9]*> is submitted to queue <" "$OUTFILE"`
    jobid=`echo $qstr | awk -F '[<>]' '{print $2}'`
    jobid_short=$jobid
    rm -f "$OUTFILE"
    echo $qstr
}

jobctl_set_paths() {
    SCRATCH_PWD="$PWD"
    SCRATCH_HOME="$HOME"
}

jobctl_set_outfiles() {
    FILE_OUT=$SCRATCH_PWD/output.$jobid
}

jobctl_move_outfiles() {
    if [ -f "$FILE_OUT" -a "$jobid" != "" ]; then
        mkdir -p $SCRATCH_PWD/results.$jobid_short
        mv $FILE_OUT $SCRATCH_PWD/results.$jobid_short
        mv $LSF_FILE $SCRATCH_PWD/results.$jobid_short
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

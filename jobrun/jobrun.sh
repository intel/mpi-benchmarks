#!/bin/sh
#set -x

if [ -z "$1" -o -z "$2" -o -z "$3" ]; then echo "Usage: " `basename $0` "NUM_NODES PROC_PER_NODE [args]"; exit 1; fi

NNODES=$1
PPN=$2
n=`expr $NNODES \* $PPN`

ARGS="$3"

# All these options can be overriden by run.options script
QUEUE=compute
TIME_LIMIT=10
RUN_SH=./run.sh
INIT_COMMANDS=""
BINARY_TO_RUN="test"

if [ -f ./run.options ]; then
    . ./run.options
fi

trap jobctl_cleanup INT TERM

echo "QUEUE=${QUEUE}; TIME_LIMIT=${TIME_LIMIT}; RUN_SH=${RUN_SH}; NNODES=${NNODES}; PPN=${PPN}"
echo "INIT_COMMANDS: $INIT_COMMANDS"
echo "RUN.SH: " `cat $RUN_SH`

if [ ! -z "$INIT_COMMANDS" ]; then
    eval "$INIT_COMMANDS"
fi

if [ "$BAD_HOSTS_FILE" != "" -a -f "$BAD_HOSTS_FILE" ]; then
	EXCLUDE_HOSTS=""
	while read h
	do
		if [ ! -z "$EXCLUDE_HOSTS" ]; then EXCLUDE_HOSTS="$EXCLUDE_HOSTS,$h"; fi
		if [ -z "$EXCLUDE_HOSTS" ]; then EXCLUDE_HOSTS="$h"; fi
	done < "$BAD_HOSTS_FILE"
fi


 
jobctl_submit
jobctl_set_paths
jobctl_set_outfiles

ncancel="0"
while [ ! -f "$FILE_OUT" ]; do
    jobctl_check_job_status
    if [ "$jobstatus" == "NONE" ]; then exit 1; fi
    if [ "$jobstatus" == "C" ]; then 
	if [ "$ncancel" -gt "5" ]; then break; fi
	ncancel=`expr $ncancel \+ 1`
    fi
    sleep 5
done

jobctl_check_job_status


rm -f __old.out
no_newstr="0"
benchmark_time="0"

cat "$FILE_OUT" | grep '>>> Nodelist ' | awk 'BEGIN{RS="[, ]"; FS=":"} /^.*:/ { print $1 }' > __nodelist

#tail -f "$FILE_OUT"
while true
do
    jobctl_check_job_done
    [ "$jobdone" == "1" ] && break

	if [ "$HANGUP_TIME" != "" ]; then
		diff -uBbw __old.out "$FILE_OUT" >& __diff.out
		nnewstr=`cat __diff.out | wc -l | awk '{print $1}'`
		if [ "$nnewstr" == "0" -a "$jobstatus" == "R" ]; then
			no_newstr=`expr "$no_newstr" \+ 1`;
		else
			no_newstr="0";
		fi
		if [ "$no_newstr" == "$HANGUP_TIME" ]; then
			echo "NO NEW LINES IN OUTPUT: $no_newstr"
			if [ "$BINARY_TO_BACKTRACE" != "" ]; then
				echo "Trying to save stack for binary \"$BINARY_TO_BACKTRACE\" on nodes:"
				for n in `cat __nodelist`; do
					ssh $n "ps aux | grep '[0-9 ][0-9]:[0-9][0-9] $BINARY_TO_BACKTRACE' | awk '{print \$2}' > pids.$n"
					ssh $n "for i in \$(cat pids.$n); do gdb -x gdb.cmd -batch --pid=\$i; done > stack.$n 2>/dev/null "
					if [ -s $SCRATCH_HOME/stack.$n ]; then echo "Stack file stack.$n created"; fi
				done
				fi
			echo "Canceling job $jobid"
			jobctl_cancel
			sleep 1
		fi
	fi
    if [ "$BENCHMARK_TIME" != "" ]; then
            cat __diff.out | eval "$BENCHMARK_TIME_STRING_CMD" > __benchmark
            if [ -s __benchmark ]; then benchmark_time=`eval "cat __benchmark | $BENCHMARK_TIME_EXTRACT_CMD"`; fi
            cat __benchmark | sed 's/^+//'
            if [ "$benchmark_time" -gt "$BENCHMARK_TIME" ]; then
                    echo "WRONG BENCHMARK TIME" 
        echo "Canceling job $jobid"
        jobctl_cancel
                    sleep 1
                    if [ "$BAD_HOSTS_FILE" != "" ]; then
                            echo "Adding these nodes to bad hosts file: \"$BAD_HOSTS_FILE\""
                            cat __nodelist "$BAD_HOSTS_FILE" | sort | uniq > __tmp
                            cat __tmp > "$BAD_HOSTS_FILE"
                    fi
            fi
    fi
    jobctl_check_job_status
#       if [ "$jobstatus" != "R" ]; then no_newstr="0"; fi
    if [ "$jobstatus" == "NONE" ]; then break; fi
    if [ "$jobstatus" == "C" ]; then break; fi
    if [ "$jobstatus" == "E" ]; then break; fi
    cp "$FILE_OUT" __old.out
    sleep 1
done

jobctl_move_outfiles


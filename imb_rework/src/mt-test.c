/* vim: set syntax=c: */
/* preprocess with: perl pp.pl test.c.in > test.c
 * pp.pl code is below: */
/*
$re_list = qr"^(@\w+@)=(.*)";
%g_list = ();

sub match {
	my ($l) = @_;
	foreach my $k (keys(%g_list)) {
		if ($l =~ /$k/) {
			foreach my $v (@{$g_list{$k}}) {
				(my $p = $l) =~ s/$k/$v/g;
				print $p;
			}
			return 1;
		}
	}
	return 0;
}

while(<>) {
	$line = $_;
	if ($line =~ /$re_list/) {
		$g_list{$1} = [split ",", $2];
	} elsif (match($line)) {
	} else {
		print $line;
	}
}
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "mpi.h"

/* arg parsing convenience macros */
#define START_WITH(s1,s2) (strncmp((s1),(s2),strlen(s2)) == 0)
#define TAIL(s1,s2) ((s1)+strlen(s2))
#define GET_ARG_SINGLE(arg,prefix,val) if(START_WITH(arg,prefix)) val=atoi(TAIL(arg,prefix));
#define GET_ARG_ARRAY(arg,prefix,val,n) \
    if(START_WITH(arg,prefix) && !(val)) { \
        char *vals = strdup(TAIL(arg,prefix)), *s; \
        int i = 0; \
        for (n = 1, s = vals; *s; s++) if (*s == ',') n++; \
        val = (int*) malloc(n*sizeof(int)); \
        s = strtok(vals, ","); \
        if (s) (val)[i++] = atoi(s); \
        while( s = strtok(NULL, ",") ) (val)[i++] = atoi(s); \
        n = i; \
        free(vals); \
    }
#define PRINT_ARG_SINGLE(val) fprintf(stderr, "DEBUG: %s = %d\n", #val, val)
#define PRINT_ARG_ARRAY(val,n) \
    do { \
        int i; \
        fprintf(stderr, "DEBUG: %s[%d] = ", #val, n); \
        if (val) \
            for (i = 0; i < n; i++) \
                fprintf(stderr, "%d%s", val[i], i==n-1 ? "" : ","); \
        fprintf(stderr, "\n"); \
    } while(0)

/* arg defaults */
#define DEF_debug 0
#define DEF_repeat 1000
#define DEF_perf 0
#define DEF_skip 0
#define DEF_mt 0
#define DEF_count "--count=1048576"
#define DEF_kind "--kind=0"
/* test kind names */
enum kind_id {
    KIND_SEND_RECV,
    KIND_PINGPONG,
    KIND_ISEND_RECV,
    KIND_SEND_IRECV,
    KIND_SENDRECV,
    KIND_BCAST,
    KIND_ALLREDUCE,
};
char *kind_name[] = {
    "SEND_RECV",
    "PINGPONG",
    "ISEND_RECV",
    "SEND_IRECV",
    "SENDRECV",
    "BCAST",
    "ALLREDUCE",
};

#define ARG_ARRAY(v) int *v; int n##v; int i##v
struct arg {
    int debug;
    int repeat;
    int perf;
    int skip;
    int mt;
    ARG_ARRAY(count);
    ARG_ARRAY(kind);
};
int test(int id, MPI_Comm *comm, struct arg *args);

int main(int argc,char *argv[])
{
    int i;
    struct arg args = {0};
    MPI_Comm *comm = NULL;

    /* apply defaults for scalars */
    args.debug = DEF_debug;
    args.repeat = DEF_repeat;
    args.perf = DEF_perf;
    args.skip = DEF_skip;
    args.mt = DEF_mt;
    /* interpret arguments */
    for (i = 0; i < argc; i++) {
        GET_ARG_SINGLE(argv[i], "--debug=", args.debug);
        GET_ARG_SINGLE(argv[i], "--repeat=", args.repeat);
        GET_ARG_SINGLE(argv[i], "--perf=", args.perf);
        GET_ARG_SINGLE(argv[i], "--skip=", args.skip);
        GET_ARG_SINGLE(argv[i], "--mt=", args.mt);
        GET_ARG_ARRAY(argv[i], "--count=", args.count, args.ncount);
        GET_ARG_ARRAY(argv[i], "--kind=", args.kind, args.nkind);
    }
    /* apply defaults for arrays */
    GET_ARG_ARRAY(DEF_count, "--count=", args.count, args.ncount);
    GET_ARG_ARRAY(DEF_kind, "--kind=", args.kind, args.nkind);
    if (args.debug) {
        PRINT_ARG_SINGLE(args.debug);
        PRINT_ARG_SINGLE(args.repeat);
        PRINT_ARG_SINGLE(args.perf);
        PRINT_ARG_SINGLE(args.skip);
        PRINT_ARG_SINGLE(args.mt);
        PRINT_ARG_ARRAY(args.count, args.ncount);
        PRINT_ARG_ARRAY(args.kind, args.nkind);
    }
    /* Init, make communicators */
    comm = (MPI_Comm*)malloc(sizeof(MPI_Comm) * (args.mt ? args.mt : 1)); if (!comm) return -1;
    if (args.mt) {
        int provided = -1;
        MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
        if (provided != MPI_THREAD_MULTIPLE) {
            fprintf(stderr, "provided != MPI_THREAD_MULTIPLE\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (i = 0; i < args.mt; i++)
            MPI_Comm_dup(MPI_COMM_WORLD, &comm[i]);
    } else {
        MPI_Init(&argc, &argv);
        comm[0] = MPI_COMM_WORLD;
    }
    if (args.debug > 1) {
        fprintf(stderr, "Init done\n");
        sleep(1);
    }

    /* Testing loop */
    for (args.icount = 0; args.icount < args.ncount; args.icount++) {
    for (args.ikind = 0; args.ikind < args.nkind; args.ikind++) {
        int ret = 0;
        if (args.mt) {
#pragma omp parallel default(shared) num_threads(args.mt) reduction(+:ret)
            ret += test(omp_get_thread_num(), comm, &args);
        } else
            ret = test(0, comm, &args);
        if (ret) break;
    } /* kind */
    } /* count */

    /* Closure */
    free(comm);
    free(args.count);
    free(args.kind);
    if (args.debug > 1) {
        fprintf(stderr, "Tests done\n");
        sleep(1);
    }
    MPI_Finalize();
    return 0;
}

/* testing convenience macros */
#define INIT_ARRAY(cond,arr,val) if (!perf && cond) for (i = 0; i < count; i++) arr[i] = (val)
#define CHECK_ARRAY(cond,arr,val) \
    if (!perf && cond) for (i = 0; i < count; i++) if( arr[i] != (val) ) { \
        if (debug) \
            fprintf(stderr,"Process %d id %d FAILED at index %d: got %d, expected %d\n", \
                    rank, id, i, arr[i], (val)); \
        fail = 1; \
        break; \
    }

#define CASE(kind,init,action,check) case kind: init; action; check; break;

#define PT2PT_1WAY(kind,action_0,action_1) \
    CASE(kind, \
        INIT_ARRAY(rank == 0, a, i+id); INIT_ARRAY(rank == 1, a, -1), \
        if (rank == 0) {action_0;} else if (rank == 1) {action_1;}, \
        CHECK_ARRAY(rank < 2, a, i+id))

/* performance convenience macros */
struct timer {
    char *op;
    double stamp;
    double acc;
    int on;
};

void timer_on(struct timer *t, char *op) {
    t->on = 1;
    t->op = op;
    t->stamp = MPI_Wtime();
}

void timer_off(struct timer *t, int repeat, int bytes) {
    double tpc = 0.0, perf = 0.0;
    if (t->on == 1) {
        t->on = 0;
        t->stamp = MPI_Wtime() - t->stamp;
        if (repeat) tpc = 1.0e6*t->stamp/(double)repeat;
        if (repeat && bytes) perf = (double)bytes/tpc;
        fprintf(stderr,"%s calls: %d, time/call: %f usec, bytes: %d, perf: %f MB/sec\n",
                t->op, repeat, tpc, bytes, perf);
    } else t->on = -1;
}

/* Test function
 * takes instance#, global settings on input
 * returns 0 if test is passed, 1 otherwise
 * */

static int test(int id, MPI_Comm *comm, struct arg *args) {
    int debug = args->debug;
    int repeat = args->repeat;
    int perf = args->perf;
    int skip = args->skip;
    int mt = args->mt;
    int count = args->count[args->icount];
    int kind = args->kind[args->ikind];
    int *a = NULL;
    int *b = NULL;
    int n, rank, size, i, j, root, left, right, fail = 0;
    struct timer timer = {0};
    char diag[1024] = {0}, arg[128] = {0};
    sprintf(arg, "count %d: ", count); strcat(diag, arg);
    sprintf(arg, "kind %d: ", kind); strcat(diag, arg);

    MPI_Comm_size(comm[id], &size);
    MPI_Comm_rank(comm[id], &rank);
    left = (size + rank - 1) % size;
    right = (rank + 1) % size;
    a = (int*)malloc(sizeof(int)*count); if(!a) return -1;
    b = (int*)malloc(sizeof(int)*count); if(!b) return -1;
    for (j = 0; j < repeat; j++) {
        if (debug > 2)
            fprintf(stderr,"Iter %d, Process %d of %d: %s BEGIN\n", j, rank, size, diag);

        if (!debug && perf && j == skip)
            timer_on(&timer, diag);
        switch (kind) {
            /* Blocking send-recv, 2 ranks */
            PT2PT_1WAY(KIND_SEND_RECV,
                    MPI_Send(a, count, MPI_INT, 1, 0, comm[id]),
                    MPI_Recv(a, count, MPI_INT, 0, 0, comm[id], MPI_STATUS_IGNORE)
                 );
            /* Pingpong, 2 ranks */
            PT2PT_1WAY(KIND_PINGPONG,
                    MPI_Send(a, count, MPI_INT, 1, 0, comm[id]);
                    MPI_Recv(a, count, MPI_INT, 1, 0, comm[id], MPI_STATUS_IGNORE),
                    MPI_Recv(a, count, MPI_INT, 0, 0, comm[id], MPI_STATUS_IGNORE);
                    MPI_Send(a, count, MPI_INT, 0, 0, comm[id])
                 );
            /* Non-blocking send, blocking recv, 2 ranks */
            PT2PT_1WAY(KIND_ISEND_RECV,
                    MPI_Request req;
                    MPI_Isend(a, count, MPI_INT, 1, 0, comm[id], &req);
                    MPI_Wait(&req, MPI_STATUS_IGNORE),
                    MPI_Recv(a, count, MPI_INT, 0, 0, comm[id], MPI_STATUS_IGNORE)
                 );
            /* Blocking send, non-blocking recv, 2 ranks */
            PT2PT_1WAY(KIND_SEND_IRECV,
                    MPI_Send(a, count, MPI_INT, 1, 0, comm[id]),
                    MPI_Request req;
                    MPI_Irecv(a, count, MPI_INT, 0, 0, comm[id], &req);
                    MPI_Wait(&req, MPI_STATUS_IGNORE)
                 );
            case KIND_SENDRECV: /* Blocking Sendrecv, ring */
                INIT_ARRAY(1, a, (rank+1)*i);
                INIT_ARRAY(1, b, -1);

                MPI_Sendrecv(a, count, MPI_INT, right, 0, b, count, MPI_INT, left, 0,
                        comm[id], MPI_STATUS_IGNORE);

                CHECK_ARRAY(1, a, (rank+1)*i);
                CHECK_ARRAY(1, b, (left+1)*i);
                break;

            case KIND_BCAST: /* Bcast */
                for (root = 0; root < size; root++) {
                    INIT_ARRAY(rank == root, a, i);
                    INIT_ARRAY(rank != root, a, -1);

                    MPI_Bcast(a, count, MPI_INT, root, comm[id]);

                    CHECK_ARRAY(1, a, i);
                }
                break;

            case KIND_ALLREDUCE: /* Allreduce */
                INIT_ARRAY(1, a, (rank+1)*i);

                MPI_Allreduce(MPI_IN_PLACE, a, count, MPI_INT, MPI_SUM, comm[id]);

                CHECK_ARRAY(1, a, size*(size+1)*i/2);
                break;
        }
        if (debug > 2)
            fprintf(stderr,"Iter %d, Process %d of %d: %s END\n", j, rank, size, diag);

        if (fail) break;
    }
    if (!debug && perf)
        timer_off(&timer, repeat-skip, count*sizeof(int));
    if (fail || debug)
        fprintf(stderr,"id: %d, %s%s\n", id, diag, fail ? "FAIL" : "PASS");
    free(a);
    free(b);
    return fail;
}

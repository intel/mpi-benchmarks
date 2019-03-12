/*****************************************************************************
 *                                                                           *
 * Copyright 2016-2019 Intel Corporation.                                    *
 *                                                                           *
 *****************************************************************************

This code is covered by the Community Source License (CPL), version
1.0 as published by IBM and reproduced in the file "license.txt" in the
"license" subdirectory. Redistribution in source and binary form, with
or without modification, is permitted ONLY within the regulations
contained in above mentioned license.

Use of the name and trademark "Intel(R) MPI Benchmarks" is allowed ONLY
within the regulations of the "License for Use of "Intel(R) MPI
Benchmarks" Name and Trademark" as reproduced in the file
"use-of-trademark-license.txt" in the "license" subdirectory.

THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT
LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. Each Recipient is
solely responsible for determining the appropriateness of using and
distributing the Program and assumes all risks associated with its
exercise of rights under this Agreement, including but not limited to
the risks and costs of program errors, compliance with applicable
laws, damage to or loss of data, programs or equipment, and
unavailability or interruption of operations.

EXCEPT AS EXPRESSLY SET FORTH IN THIS AGREEMENT, NEITHER RECIPIENT NOR
ANY CONTRIBUTORS SHALL HAVE ANY LIABILITY FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING
WITHOUT LIMITATION LOST PROFITS), HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OR
DISTRIBUTION OF THE PROGRAM OR THE EXERCISE OF ANY RIGHTS GRANTED
HEREUNDER, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF
YOUR JURISDICTION. It is licensee's responsibility to comply with any
export regulations applicable in licensee's jurisdiction. Under
CURRENT U.S. export regulations this software is eligible for export
from the U.S. and can be downloaded by or otherwise exported or
reexported worldwide EXCEPT to U.S. embargoed destinations which
include Cuba, Iraq, Libya, North Korea, Iran, Syria, Sudan,
Afghanistan and any other country to which the U.S. has embargoed
goods and services.

 ***************************************************************************
*/

#pragma once

#include <algorithm>

#ifdef MPIIO
static int do_nonblocking_;
#endif
typedef void (*original_benchmark_func_t)(struct comm_info* c_info, int size,
                struct iter_schedule* ITERATIONS, MODES RUN_MODE, double* time);

enum descr_t {
    REDUCTION, SELECT_SOURCE,
    GET, PUT, NO,
    SINGLE_TRANSFER, PARALLEL_TRANSFER, COLLECTIVE, SINGLE_ELEMENT_TRANSFER, MULT_PASSIVE_TRANSFER,
    PARALLEL_TRANSFER_MSG_RATE, SYNC,
    SCALE_TIME_HALF, SCALE_BW_DOUBLE, SCALE_BW_FOUR,
    SENDBUF_SIZE_I, SENDBUF_SIZE_2I, SENDBUF_SIZE_NP_I, SENDBUF_SIZE_0,
    RECVBUF_SIZE_I, RECVBUF_SIZE_2I, RECVBUF_SIZE_3I, RECVBUF_SIZE_NP_I, RECVBUF_SIZE_0,
    HAS_ROOT,
    BIDIR_1,
    N_MODES_1,
    NON_AGGREGATE,
    NONBLOCKING,
    NTIMES_3, NTIMES_2,
    INDV_BLOCK, PRIVATE, SHARED, EXPLICIT,
    DEFAULT
};

struct GLOBALS {
    int NP_min;
    int NP, iter, size, ci_np, imod;
    int header, MAXMSG;
    int x_sample, n_sample;
    Type_Size unit_size;
    double time[MAX_TIME_ID];
};

struct Bmark_descr {
    Bmark_descr() : stop_iterations(false), sample_time(0) {}
    typedef std::set<descr_t> descr_set;
    descr_set flags;
    std::vector<std::string> comments;
    std::vector<const char *> cmt;
    bool stop_iterations;
    int time_limit[2];
    double sample_time;
    BTYPES descr2type(descr_t t) {
        switch(t) {
            case SINGLE_TRANSFER:
                return SingleTransfer;
            case PARALLEL_TRANSFER:
                return ParallelTransfer;
            case COLLECTIVE:
                return Collective;
            case PARALLEL_TRANSFER_MSG_RATE:
                return ParallelTransferMsgRate;
            case SYNC:
                return Sync;
            case SINGLE_ELEMENT_TRANSFER:
                return SingleElementTransfer;
            case MULT_PASSIVE_TRANSFER:
                return MultPassiveTransfer;
            default:
                return BTYPE_INVALID;
        }
        return BTYPE_INVALID;
    }
    size_t descr2len(descr_t t, size_t i, size_t np) {
        switch(t) {
            case SENDBUF_SIZE_I:
                return i;
            case SENDBUF_SIZE_2I:
                return (size_t)2 * i;
            case SENDBUF_SIZE_NP_I:
                return np * i;
            case SENDBUF_SIZE_0:
                return 0;
            case RECVBUF_SIZE_I:
                return i;
            case RECVBUF_SIZE_2I:
                return (size_t)2 * i;
            case RECVBUF_SIZE_3I:
                return (size_t)3 * i;
            case RECVBUF_SIZE_NP_I:
                return np * i;
            case RECVBUF_SIZE_0:
                return 0;
            default:
                throw std::logic_error("descr2len: unknown len");
        }
        throw std::logic_error("descr2len: unknown len");
        return 0;
    }

    DIRECTION descr2access(descr_t t) {
        switch(t) {
            case GET:
                return get;
            case PUT:
                return put;
            case NO:
                return no;
            default:
                throw std::logic_error("descr2access: unknown access");
        }
        throw std::logic_error("descr2access: unknown access");
    }
#ifdef MPIIO
    POSITIONING descr2fpointer(descr_t t) {
        switch(t) {
            case INDV_BLOCK:
                return indv_block;
            case PRIVATE:
                return priv;
            case SHARED:
                return shared;
            case EXPLICIT:
                return explic;
            default:
                throw std::logic_error("descr2position: unknown fpointer");
        }
    }
#endif
    bool is_default() {
        return flags.count(DEFAULT) > 0;
    }

    bool IMB_set_bmark(struct Bench* Bmark, original_benchmark_func_t fn)
    {
        bool result = true;
        Bmark->N_Modes = 1;
        Bmark->RUN_MODES[0].AGGREGATE   =-1;
        Bmark->RUN_MODES[0].NONBLOCKING = 0;

        Bmark->reduction = (flags.count(REDUCTION) > 0);
        Bmark->Ntimes = 1;
        bool found = false;
#ifdef MPI1
        Bmark->select_source = (flags.count(SELECT_SOURCE) > 0);
#endif /*MPI1*/

#if (defined RMA || defined EXT || defined MPIIO)
        Bmark->N_Modes = flags.count(N_MODES_1) > 0 ? 1 : 2;
#ifdef RMA
        Bmark->RUN_MODES[0].AGGREGATE   = 0;
        Bmark->RUN_MODES[1].AGGREGATE   = 1;
#else
        Bmark->RUN_MODES[0].AGGREGATE   = 1;
        Bmark->RUN_MODES[1].AGGREGATE   = 0;
#endif /*RMA*/
        Bmark->RUN_MODES[0].NONBLOCKING = 0;
        Bmark->RUN_MODES[1].NONBLOCKING = 0;
        Bmark->RUN_MODES[0].BIDIR       = 0;
        Bmark->RUN_MODES[1].BIDIR       = 0;
#ifdef MPIIO
        descr_set fpointer;
        fpointer.insert(INDV_BLOCK);
        fpointer.insert(SHARED);
        fpointer.insert(PRIVATE);
        fpointer.insert(EXPLICIT);
        for (descr_set::iterator it = fpointer.begin(); it != fpointer.end(); ++it) {
            if (flags.count(*it)) {
                if (found)
                    result = false;
                Bmark->fpointer = descr2fpointer(*it);
                found = true;
            }
        }
        if (flags.count(NTIMES_2) > 0) {
            Bmark->Ntimes = 2;
        }

#endif /*MPIIO*/
        if (flags.count(NON_AGGREGATE)) {
            Bmark->RUN_MODES[0].AGGREGATE = -1;
        }

        if (flags.count(BIDIR_1)) {
            Bmark->RUN_MODES[0].BIDIR = 1;
            Bmark->RUN_MODES[1].BIDIR = 1;
        }

        descr_set access;
        access.insert(GET);
        access.insert(PUT);
        access.insert(NO);
        for (descr_set::iterator it = access.begin(); it != access.end(); ++it) {
            if (flags.count(*it)) {
                if (found)
                    result = false;
                Bmark->access = descr2access(*it);
                found = true;
            }
        }
        if (!found)
            result = false;
#endif /*RMA || EXT*/

        if (flags.count(NONBLOCKING)) {
            Bmark->RUN_MODES[0].NONBLOCKING = 1;
#ifdef MPIIO
           do_nonblocking_ = 1;
#endif
        }
        if (flags.count(NTIMES_3) > 0) {
            Bmark->Ntimes = 3;
        }
        Bmark->Benchmark = fn;
        for (size_t i = 0; i < comments.size(); i++) {
            cmt.push_back(comments[i].c_str());
        }
        cmt.push_back(NULL);
        Bmark->bench_comments = const_cast<char **>(&cmt[0]);

        descr_set types;
        types.insert(SINGLE_TRANSFER);
        types.insert(PARALLEL_TRANSFER);
        types.insert(COLLECTIVE);
        types.insert(PARALLEL_TRANSFER_MSG_RATE);
        types.insert(SYNC);
        types.insert(SINGLE_ELEMENT_TRANSFER);
        types.insert(MULT_PASSIVE_TRANSFER);
        for (descr_set::iterator it = types.begin(); it != types.end(); ++it) {
            if (flags.count(*it)) {
                if (found)
                    result = false;
                Bmark->RUN_MODES[1].type = Bmark->RUN_MODES[0].type = descr2type(*it);
                found = true;
            }
        }
        if (!found)
            result = false;
        Bmark->scale_time = 1.0;
        Bmark->scale_bw = 1.0;
        if (flags.count(SCALE_TIME_HALF)) {
            Bmark->scale_time = 0.5;
        }
        if (flags.count(SCALE_BW_DOUBLE)) {
            Bmark->scale_bw = 2.0;
        }
        if (flags.count(SCALE_BW_FOUR)) {
            Bmark->scale_bw = 4.0;
        }

#if (defined RMA || defined MPIIO)
        Bmark->RUN_MODES[1].type = Bmark->RUN_MODES[0].type;
#endif /*RMA*/

        return result;
    }

    smart_ptr<Scope> helper_init_scope(struct comm_info &c_info,
                                       struct Bench* Bmark, GLOBALS &glob) {
        NPLenCombinedScope &scope = *(new NPLenCombinedScope);
        int len = 0;
        int iter = 0;
        bool stop = false;
        while (true) {
            if (stop)
                break;
            if (Bmark->RUN_MODES[0].type == SingleElementTransfer) {
                    /* just one size needs to be tested (the size of one element) */
                MPI_Type_size(c_info.red_data_type,&len);
            } else {
                // --- helper_get_next_size(c_info, glob);
                if (c_info.n_lens > 0) {
                    len = c_info.msglen[iter];
                } else {
                    if( iter == 0 ) {
                        if (!c_info.zero_size) {
                            iter++;
                            continue;
                        }
                        len = 0;
                    } else if (iter == 1) {
                        len = ((1<<c_info.min_msg_log) + glob.unit_size - 1)/glob.unit_size*glob.unit_size;
#ifdef EXT
                        len = std::min(len, (int) sizeof(assign_type));
#endif
                    } else {
                        len = std::min(glob.MAXMSG, len + len);
                    }
                }
            }

            // --- helper_adjust_size(c_info, glob);
            if (len > glob.MAXMSG) {
                len = glob.MAXMSG;
            }
            len = (len + glob.unit_size - 1)/glob.unit_size*glob.unit_size;
            // --- helper_post_step(glob, BMark);
            iter++;
            if (Bmark->RUN_MODES[0].type == Sync || Bmark->RUN_MODES[0].type == SingleElementTransfer) {
                stop = true;
            }
            if( Bmark->RUN_MODES[0].type == Sync ) {
                len = glob.MAXMSG;
                iter = c_info.n_lens - 1;
            }
            scope.add_len(len);
            if (!(((c_info.n_lens == 0 && len < glob.MAXMSG ) ||
                 (c_info.n_lens > 0  && iter < c_info.n_lens))))
                break;
        }

        {
            int &NP_min = glob.NP_min;
            int &ci_np = c_info.w_num_procs;
            if (Bmark->RUN_MODES[0].type == ParallelTransferMsgRate) {
                ci_np -= ci_np % 2;
                NP_min += NP_min % 2;
            }
            int NP = std::max(1, std::min(ci_np, NP_min));
            bool do_it = true;
            if (Bmark->RUN_MODES[0].type == SingleTransfer) {
#ifdef MPIIO
                    NP = 1;
#else
                NP = std::min(2, ci_np);
#endif
            }
            while (do_it) {
//                std::cout << ">> " << ci_np << " " << NP << std::endl;
                scope.add_np(NP);

                // CALCULATE THE NUMBER OF PROCESSES FOR NEXT STEP
                if (NP >= ci_np) { do_it = false; }
                else {
                    NP = std::min(NP + NP, ci_np);
                }
#ifdef MPIIO
                if (Bmark->RUN_MODES[0].type == SingleTransfer) {
                        do_it = false; 
                }
#endif
            }
        }
        scope.add_nmodes(Bmark->N_Modes);
        scope.commit();
        return smart_ptr<Scope>(&scope);
    }

    void IMB_init_buffers_iter(struct comm_info* c_info, struct iter_schedule* ITERATIONS,
                               struct Bench* Bmark, MODES BMODE, int iter, int size)
    {

    /* IMB 3.1 << */
        size_t s_len, r_len, s_alloc, r_alloc;
        int init_size, irep, i_s, i_r, x_sample;
        int asize = (int) sizeof(assign_type);


//----------------------------------------------------------------------
        const bool root_based = (flags.count(HAS_ROOT) > 0);
//----------------------------------------------------------------------
//
//
//
//
// --- STEP 1: set x_sample and ITERATIONS->n_sample
        x_sample = BMODE->AGGREGATE ? ITERATIONS->msgspersample : ITERATIONS->msgs_nonaggr;

        /* July 2002 fix V2.2.1: */
#if (defined EXT || defined MPIIO || RMA)
        if( Bmark->access==no ) x_sample=ITERATIONS->msgs_nonaggr;
#endif
        Bmark->sample_failure = 0;

        init_size = std::max(size, asize);

        if (c_info->rank < 0) {
            return;
        }

        if (ITERATIONS->iter_policy == imode_off) {
            ITERATIONS->n_sample = x_sample = ITERATIONS->msgspersample;
        } else if ((ITERATIONS->iter_policy == imode_multiple_np) ||
                   (ITERATIONS->iter_policy == imode_auto && root_based)) {
            /* n_sample for benchmarks with uneven distribution of works
               must be greater or equal and multiple to num_procs.
               The formula below is a negative leg of hyperbola.
               It's moved and scaled relative to max message size
               and initial n_sample subject to multiple to num_procs.
            */
            double d_n_sample = ITERATIONS->msgspersample;
            int max_msg_size = 1<<c_info->max_msg_log;
            int tmp = (int)(d_n_sample*max_msg_size/(c_info->num_procs*init_size+max_msg_size)+0.5);
            ITERATIONS->n_sample = x_sample = std::max(tmp-tmp%c_info->num_procs, c_info->num_procs);
        } else {
            ITERATIONS->n_sample = (size > 0)
                             ? std::max(1, std::min(ITERATIONS->overall_vol / size, x_sample))
                             : x_sample;
        }
// --- STEP 2: set s_len and r_len
//---------------------------------------------------------------------------------------------------
        bool result = true;
        {
            descr_set types;
            types.insert(SENDBUF_SIZE_I);
            types.insert(SENDBUF_SIZE_2I);
            types.insert(SENDBUF_SIZE_NP_I);
            types.insert(SENDBUF_SIZE_0);
            bool found = false;
            for (descr_set::iterator it = types.begin(); it != types.end(); ++it) {
                if (flags.count(*it)) {
                    if (found)
                        result = false;
                    s_len = descr2len(*it, (size_t)init_size,  (size_t)c_info->num_procs);
                    found = true;
                }
            }
            if (!found)
                result = false;
        }
        {
            descr_set types;
            types.insert(RECVBUF_SIZE_I);
            types.insert(RECVBUF_SIZE_2I);
            types.insert(RECVBUF_SIZE_3I);
            types.insert(RECVBUF_SIZE_NP_I);
            types.insert(RECVBUF_SIZE_0);
            bool found = false;
            for (descr_set::iterator it = types.begin(); it != types.end(); ++it) {
                if (flags.count(*it)) {
                    if (found)
                        result = false;
                    r_len = descr2len(*it, (size_t)init_size,  (size_t)c_info->num_procs);
                    found = true;
                }
            }
            if (!found)
                result = false;
        }
        if (!result) {
            throw std::logic_error("wrong recv or send buffer requirement description on a benchmark");
        }
//        printf(">> s_len=%ld, r_len=%ld\n", s_len, r_len);
//---------------------------------------------------------------------------------------------------
// --- STEP 3: set s_alloc and r_alloc AND all these ITERATIONS->s_offs,r_offs,...
//---------------------------------------------------------------------------------------------------

        /* IMB 3.1: new memory management for -off_cache */
        if (BMODE->type == Sync) {
            ITERATIONS->use_off_cache=0;
            ITERATIONS->n_sample=x_sample;
        } else {
#ifdef MPIIO
            ITERATIONS->use_off_cache=0;
#else
            ITERATIONS->use_off_cache = ITERATIONS->off_cache;
#endif
            if (ITERATIONS->off_cache) {
                if ( ITERATIONS->cache_size > 0) {
                    size_t cls = (size_t) ITERATIONS->cache_line_size;
                    size_t ofs = ( (s_len + cls - 1) / cls + 1 ) * cls;
                    ITERATIONS->s_offs = ofs;
                    ITERATIONS->s_cache_iter = std::min((int)(ITERATIONS->n_sample), (int)((2*ITERATIONS->cache_size*CACHE_UNIT+ofs-1)/ofs));
                    ofs = ( ( r_len + cls -1 )/cls + 1 )*cls;
                    ITERATIONS->r_offs = ofs;
                    ITERATIONS->r_cache_iter = std::min((int)(ITERATIONS->n_sample), (int)((2*ITERATIONS->cache_size*CACHE_UNIT+ofs-1)/ofs));
                } else {
                    ITERATIONS->s_offs=ITERATIONS->r_offs=0;
                    ITERATIONS->s_cache_iter=ITERATIONS->r_cache_iter=1;
                }
            }
        }

#ifdef MPIIO
        s_alloc = s_len;
        r_alloc = r_len;
#else
        if( ITERATIONS->use_off_cache ) {
            s_alloc = std::max(s_len,ITERATIONS->s_cache_iter*ITERATIONS->s_offs);
            r_alloc = std::max(r_len,ITERATIONS->r_cache_iter*ITERATIONS->r_offs);
        } else {
            s_alloc = s_len;
            r_alloc = r_len;
        }
#endif

// --- STEP 4: detect too much memory situation
//--------------------------------------------------------------------------------
        c_info->used_mem = 1.f*(s_alloc+r_alloc)/MEM_UNIT;

#ifdef DEBUG
        {
            size_t mx, mu;

            mx = (size_t) MEM_UNIT*c_info->max_mem;
            mu = (size_t) MEM_UNIT*c_info->used_mem;

            DBG_I3("Got send / recv lengths; iters ",s_len,r_len,ITERATIONS->n_sample);
            DBG_I2("max  / used memory ",mx,mu);
            DBG_I2("send / recv offsets ",ITERATIONS->s_offs, ITERATIONS->r_offs);
            DBG_I2("send / recv cache iterations ",ITERATIONS->s_cache_iter, ITERATIONS->r_cache_iter);
            DBG_I2("send / recv buffer allocations ",s_alloc, r_alloc);
            DBGF_I2("Got send / recv lengths ",s_len,r_len);
            DBGF_I2("max  / used memory ",mx,mu);
            DBGF_I2("send / recv offsets ",ITERATIONS->s_offs, ITERATIONS->r_offs);
            DBGF_I2("send / recv cache iterations ",ITERATIONS->s_cache_iter, ITERATIONS->r_cache_iter);
            DBGF_I2("send / recv buffer allocations ",s_alloc, r_alloc);
        }
#endif

        if( c_info->used_mem > c_info->max_mem ) {
            Bmark->sample_failure=SAMPLE_FAILED_MEMORY;
            return;
        }


// --- call IMB_set_buf, IMB_init_transfer
// -------------------------------------------------------------------------------------
        if (s_alloc > 0  && r_alloc > 0) {
            if (ITERATIONS->use_off_cache) {
                IMB_alloc_buf(c_info, (char *)"IMB_init_buffers_iter 1", s_alloc, r_alloc);
                IMB_set_buf(c_info, c_info->rank, 0, s_len-1, 0, r_len-1);

                for (irep = 1; irep < ITERATIONS->s_cache_iter; irep++) {
                    i_s = irep % ITERATIONS->s_cache_iter;
                    memcpy((void*)((char*)c_info->s_buffer + i_s * ITERATIONS->s_offs), c_info->s_buffer, s_len);
                }

                for (irep = 1; irep < ITERATIONS->r_cache_iter; irep++) {
                    i_r = irep % ITERATIONS->r_cache_iter;
                    memcpy((void*)((char*)c_info->r_buffer + i_r * ITERATIONS->r_offs), c_info->r_buffer, r_len);
                }
            } else {
                IMB_set_buf(c_info, c_info->rank, 0, s_alloc-1, 0, r_alloc-1);
            }
        }

        IMB_init_transfer(c_info, Bmark, size, (MPI_Aint) std::max(s_alloc, r_alloc));


// --- change  ITERATIONS->n_sample
// -------------------------------------------------------------------------------------
//
        /* Determine #iterations if dynamic adaptation requested */
        if ((ITERATIONS->iter_policy == imode_dynamic) || (ITERATIONS->iter_policy == imode_auto && !root_based)) {
            double time[MAX_TIME_ID];
            int acc_rep_test, t_sample;
            int selected_n_sample = ITERATIONS->n_sample;

            memset(time, 0, MAX_TIME_ID);
            if (iter == 0 || BMODE->type == Sync) {
                ITERATIONS->n_sample_prev = ITERATIONS->msgspersample;
                if (c_info->n_lens > 0) {
                    memset(ITERATIONS->numiters, 0, c_info->n_lens);
                }
            }

            /* first, run 1 iteration only */
            ITERATIONS->n_sample=1;
#ifdef MPI1
            c_info->select_source = Bmark->select_source;
#endif
            Bmark->Benchmark(c_info,size,ITERATIONS,BMODE,&time[0]);

            time[1] = time[0];

#ifdef MPIIO
            if( Bmark->access != no) {
                MPI_ERRHAND(MPI_File_seek(c_info->fh, 0 ,MPI_SEEK_SET));

                if( Bmark->fpointer == shared) {
                    MPI_ERRHAND(MPI_File_seek_shared(c_info->fh, 0 ,MPI_SEEK_SET));
                }
            }
#endif /*MPIIO*/

            MPI_Allreduce(&time[1], &time[0], 1, MPI_DOUBLE, MPI_MAX, c_info->communicator);

            { /* determine rough #repetitions for a run time of 1 sec */
                int rep_test = 1;
                if (time[0] < (1.0 / MSGSPERSAMPLE)) {
                    rep_test = MSGSPERSAMPLE;
                } else if ((time[0] < 1.0)) {
                    rep_test = (int)(1.0 / time[0] + 0.5);
                }

                MPI_Allreduce(&rep_test, &acc_rep_test, 1, MPI_INT, MPI_MAX, c_info->communicator);
            }

            ITERATIONS->n_sample = std::min(selected_n_sample, acc_rep_test);

            if (ITERATIONS->n_sample > 1) {
#ifdef MPI1
                c_info->select_source = Bmark->select_source;
#endif
                Bmark->Benchmark(c_info,size,ITERATIONS,BMODE,&time[0]);
                time[1] = time[0];
#ifdef MPIIO
                if( Bmark->access != no) {
                    MPI_ERRHAND(MPI_File_seek(c_info->fh, 0 ,MPI_SEEK_SET));

                    if ( Bmark->fpointer == shared) {
                        MPI_ERRHAND(MPI_File_seek_shared(c_info->fh, 0 ,MPI_SEEK_SET));
                    }
                }
#endif /*MPIIO*/

                MPI_Allreduce(&time[1], &time[0], 1, MPI_DOUBLE, MPI_MAX, c_info->communicator);
            }

            {
                float val = (float) (1+ITERATIONS->secs/time[0]);
                t_sample = (time[0] > 1.e-8 && (val <= (float) 0x7fffffff))
                            ? (int)val
                            : selected_n_sample;
            }

            if (c_info->n_lens>0 && BMODE->type != Sync) {
                // check monotonicity with msg sizes
                int i;
                for (i = 0; i < iter; i++) {
                    t_sample = ( c_info->msglen[i] < size )
                                ? std::min(t_sample,ITERATIONS->numiters[i])
                                : std::max(t_sample,ITERATIONS->numiters[i]);
                }
                ITERATIONS->n_sample = ITERATIONS->numiters[iter] = std::min(selected_n_sample, t_sample);
            } else {
                ITERATIONS->n_sample = std::min(selected_n_sample,
                                            std::min(ITERATIONS->n_sample_prev, t_sample));
            }

            MPI_Bcast(&ITERATIONS->n_sample, 1, MPI_INT, 0, c_info->communicator);

#ifdef DEBUG
            {
                int usec=time*1000000;

                DBGF_I2("Checked time with #iters / usec ",acc_rep_test,usec);
                DBGF_I1("=> # samples, aligned with previous ",t_sample);
                DBGF_I1("final #samples ",ITERATIONS->n_sample);
            }
#endif

// --- call Benchmark
// -------------------------------------------------------------------------------------
//
        } else { /*if( (ITERATIONS->iter_policy == imode_dynamic) || (ITERATIONS->iter_policy == imode_auto && !root_based) )*/
            double time[MAX_TIME_ID];
            Bmark->Benchmark(c_info,size,ITERATIONS,BMODE,&time[0]);
        }

// --- save n_sample_prev
// -------------------------------------------------------------------------------------
//
         ITERATIONS->n_sample_prev=ITERATIONS->n_sample;

    /* >> IMB 3.1  */

    }

    void helper_sync_globals_1(comm_info &c_info, GLOBALS &glob, struct Bench *Bmark) {
        // NP_min is already initialized by IMB_basic_input

#if (defined EXT || defined MPIIO || RMA)
        if (Bmark->N_Modes == 2) {
            int choice_aggregate_mode = c_info.aggregate_mode;
            if (choice_aggregate_mode == AM_TURN_OFF) {
                Bmark->N_Modes                  = 1;
                Bmark->RUN_MODES[0].AGGREGATE   = AM_TURN_OFF;
            } else if (choice_aggregate_mode == AM_TURN_ON) {
                Bmark->N_Modes                  = 1;
                Bmark->RUN_MODES[0].AGGREGATE   = AM_TURN_ON;
            }
        }
#endif

        glob.ci_np = c_info.w_num_procs;
        if (Bmark->RUN_MODES[0].type == ParallelTransferMsgRate) {
            glob.ci_np -= glob.ci_np % 2;
            glob.NP_min += glob.NP_min % 2;
        }
        glob.NP=std::max(1,std::min(glob.ci_np,glob.NP_min));
        if (Bmark->RUN_MODES[0].type == SingleTransfer ||
            Bmark->RUN_MODES[0].type == SingleElementTransfer) {
#ifdef MPIIO
            glob.NP = 1;
#else
            glob.NP = (std::min(2,glob.ci_np));
#endif /*MPIIO*/
        }
#ifdef RMA
        if (Bmark->RUN_MODES[0].type == MultPassiveTransfer) {
            /* Just sanity check */
            if (c_info.num_procs > 1) {
                Bmark->scale_bw = (double)c_info.num_procs - 1;
            }
        }
#endif /*RMA*/
#ifdef EXT
        MPI_Type_size(c_info.red_data_type, &glob.unit_size);
#else
        if (Bmark->reduction ||
            Bmark->RUN_MODES[0].type == SingleElementTransfer) {
            MPI_Type_size(c_info.red_data_type,&glob.unit_size);
        } else {
            MPI_Type_size(c_info.s_data_type,&glob.unit_size);
        }
#endif /*EXT*/
    }

    void helper_sync_globals_2(comm_info &c_info, GLOBALS &glob, struct Bench *Bmark) {
        glob.MAXMSG=(1<<c_info.max_msg_log)/glob.unit_size * glob.unit_size;
        glob.header=1;
        Bmark->sample_failure = 0;
        sample_time = MPI_Wtime();
        time_limit[0] = time_limit[1] = 0;
        Bmark->success = 1;
#ifdef MPI1
        c_info.select_source = Bmark->select_source;
#endif
        stop_iterations = false;
        glob.iter = 0;
        glob.size = 0;
        if (Bmark->RUN_MODES[0].type == SingleElementTransfer) {
            /* just one size needs to be tested (the size of one element) */
            MPI_Type_size(c_info.red_data_type, &glob.size);
        }
//        if (Bmark->RUN_MODES[0].type == BTYPE_INVALID)
//            stop_iterations = true;
    }

    void helper_time_check(comm_info &c_info, GLOBALS &,
                           Bench *Bmark, iter_schedule &ITERATIONS) {
        if (!Bmark->sample_failure) {
            time_limit[1] = 0;
            if (c_info.rank >= 0) {
                time_limit[1] = (MPI_Wtime() - sample_time < std::max(std::max(c_info.n_lens, c_info.max_msg_log - c_info.min_msg_log) - 1, 1) * ITERATIONS.secs) ? 0 : 1;
            }
        }
        MPI_Allreduce(&time_limit[1], &time_limit[0], 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        if (time_limit[0]) {
            Bmark->sample_failure = SAMPLE_FAILED_TIME_OUT;
            stop_iterations = true;
        }
        return;
    }
};

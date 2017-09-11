/*****************************************************************************
 *                                                                           *
 * Copyright (c) 2003-2016 Intel Corporation.                                *
 * All rights reserved.                                                      *
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

For more documentation than found here, see

[1] doc/ReadMe_IMB.txt 

[2] Intel (R) MPI Benchmarks
    Users Guide and Methodology Description
    In 
    doc/IMB_Users_Guide.pdf

 File: IMB_rma_atomic.c 

 Implemented functions: 

 IMB_rma_accumulate;
 IMB_rma_get_accumulate;
 IMB_rma_fetch_and_op;
 IMB_rma_compare_and_swap;
 
 ***************************************************************************/

/*-----------------------------------------------------------*/

#include "IMB_declare.h"
#include "IMB_benchmark.h"
#include "IMB_prototypes.h"

void IMB_rma_accumulate (struct comm_info* c_info, int size,  
                         struct iter_schedule* iterations,
                         MODES run_mode, double* time)
{
    double res_time = -1.;
    Type_Size s_size,r_size;
    int s_num, r_num;
    /* IMB 3.1 << */
    int r_off;
    int i;
    int root = c_info->pair1;
    ierr = 0;
     
    if (c_info->rank < 0)
    {
        *time = res_time;
        return;
    }    
    
    MPI_Type_size(c_info->red_data_type,&s_size);
    s_num=size/s_size;
    r_size=s_size;
    r_num=s_num;
    r_off=iterations->r_offs/r_size;

    for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0)
    {
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE)
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Accumulate(
                        (char*)c_info->s_buffer+i%iterations->s_cache_iter*iterations->s_offs,
                        s_num, c_info->red_data_type, root, 
                        i%iterations->r_cache_iter*r_off, r_num, 
                        c_info->red_data_type, c_info->op_type, c_info->WIN );
                MPI_ERRHAND(ierr);
            }
            ierr = MPI_Win_flush(root, c_info->WIN);
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }    
        else if ( !run_mode->AGGREGATE )    
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Accumulate(
                        (char*)c_info->s_buffer+i%iterations->s_cache_iter*iterations->s_offs,
                        s_num, c_info->red_data_type, root, 
                        i%iterations->r_cache_iter*r_off, r_num, 
                        c_info->red_data_type, c_info->op_type, c_info->WIN );
                MPI_ERRHAND(ierr);

                ierr = MPI_Win_flush(root, c_info->WIN);
                MPI_ERRHAND(ierr);
            }
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time; 
    return;
}    


void IMB_rma_get_accumulate (struct comm_info* c_info, int size,  
                             struct iter_schedule* iterations,
                             MODES run_mode, double* time)
{
    double res_time = -1.;
    Type_Size s_size,r_size;
    int s_num, r_num;
    int r_off;
    int i;
    int root = c_info->pair1;
    ierr = 0;
     
    if (c_info->rank < 0)
    {
        *time = res_time;
        return;
    }    
    
    MPI_Type_size(c_info->red_data_type,&s_size);
    s_num=size/s_size;
    r_size=s_size;
    r_num=s_num;
    r_off=iterations->r_offs/r_size;

    for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0)
    {
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE)
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Get_accumulate(
                        (char*)c_info->s_buffer+i%iterations->s_cache_iter*iterations->s_offs,
                        s_num, c_info->red_data_type, 
                        (char*)c_info->r_buffer+i%iterations->r_cache_iter*iterations->r_offs,
                        r_num, c_info->red_data_type, 
                        root, i%iterations->r_cache_iter*r_off, r_num, 
                        c_info->red_data_type, c_info->op_type, c_info->WIN );
                MPI_ERRHAND(ierr);
            }
            ierr = MPI_Win_flush(root, c_info->WIN);
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }    
        else if ( !run_mode->AGGREGATE )    
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Get_accumulate(
                        (char*)c_info->s_buffer+i%iterations->s_cache_iter*iterations->s_offs,
                        s_num, c_info->red_data_type, 
                        (char*)c_info->r_buffer+i%iterations->r_cache_iter*iterations->r_offs,
                        r_num, c_info->red_data_type, 
                        root, i%iterations->r_cache_iter*r_off, r_num, 
                        c_info->red_data_type, c_info->op_type, c_info->WIN );
                MPI_ERRHAND(ierr);

                ierr = MPI_Win_flush(root, c_info->WIN);
                MPI_ERRHAND(ierr);
            }
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time; 
    return;
}    

void IMB_rma_fetch_and_op (struct comm_info* c_info, int size,  
                           struct iter_schedule* iterations,
                           MODES run_mode, double* time)
{
    double res_time = -1.;
    Type_Size r_size;
    int r_off;
    int i;
    int root = c_info->pair1;
    ierr = 0;
     
    if (c_info->rank < 0)
    {
        *time = res_time;
        return;
    }    
    
    MPI_Type_size(c_info->red_data_type,&r_size);
    r_off=iterations->r_offs/r_size;

    for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);

    if (c_info->rank == c_info->pair0)
    {
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE)
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Fetch_and_op(
                        (char*)c_info->s_buffer+i%iterations->s_cache_iter*iterations->s_offs,
                        (char*)c_info->r_buffer+i%iterations->r_cache_iter*iterations->r_offs,
                        c_info->red_data_type, root, 
                        i%iterations->r_cache_iter*r_off, c_info->op_type, c_info->WIN );
                MPI_ERRHAND(ierr);
            }
            ierr = MPI_Win_flush(root, c_info->WIN);
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }    
        else if ( !run_mode->AGGREGATE )    
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Fetch_and_op(
                        (char*)c_info->s_buffer+i%iterations->s_cache_iter*iterations->s_offs,
                        (char*)c_info->r_buffer+i%iterations->r_cache_iter*iterations->r_offs,
                        c_info->red_data_type, root, 
                        i%iterations->r_cache_iter*r_off, c_info->op_type, c_info->WIN );
                MPI_ERRHAND(ierr);

                ierr = MPI_Win_flush(root, c_info->WIN);
                MPI_ERRHAND(ierr);
            }
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time; 
    return;
}

void IMB_rma_compare_and_swap (struct comm_info* c_info, int size,  
                               struct iter_schedule* iterations,
                               MODES run_mode, double* time)
{
    double res_time = -1.;
    int root = c_info->pair1;
    int s_size;
    int i;
    void *comp_b, *orig_b, *res_b; 
    MPI_Datatype data_type = MPI_INT;
    ierr = 0;
          
    if (c_info->rank < 0)
    {
        *time = res_time;
        return;
    }    
    
    MPI_Type_size(data_type,&s_size);
    for(i=0; i<N_BARR; i++) MPI_Barrier(c_info->communicator);
    

    if (c_info->rank == c_info->pair0)
    {
        /* use r_buffer for all buffers required by compare_and_swap, because 
         * on all ranks r_buffer is zero-initialized in IMB_set_buf function */
        orig_b = (char*)c_info->r_buffer + s_size*2;
        comp_b = (char*)c_info->r_buffer + s_size;
        res_b  = c_info->r_buffer;
 
        MPI_Win_lock(MPI_LOCK_SHARED, root, 0, c_info->WIN);
        if (run_mode->AGGREGATE)
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Compare_and_swap(
                        (char*)orig_b + i%iterations->r_cache_iter*iterations->r_offs,
                        (char*)comp_b + i%iterations->r_cache_iter*iterations->r_offs,
                        (char*)res_b  + i%iterations->r_cache_iter*iterations->r_offs,
                        data_type, root, i%iterations->r_cache_iter*iterations->r_offs, 
                        c_info->WIN );
                MPI_ERRHAND(ierr);
            }
            ierr = MPI_Win_flush(root, c_info->WIN);
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }    
        else if ( !run_mode->AGGREGATE )    
        {
            res_time = MPI_Wtime();
            for (i = 0; i < iterations->n_sample; i++)
            {
                ierr = MPI_Compare_and_swap(
                        (char*)orig_b + i%iterations->s_cache_iter*iterations->s_offs,
                        (char*)comp_b + i%iterations->s_cache_iter*iterations->s_offs,
                        (char*)res_b  + i%iterations->r_cache_iter*iterations->r_offs,
                        data_type, root, i%iterations->r_cache_iter*iterations->r_offs,
                        c_info->WIN );
                MPI_ERRHAND(ierr);

                ierr = MPI_Win_flush(root, c_info->WIN);
                MPI_ERRHAND(ierr);
            }
            res_time = (MPI_Wtime() - res_time)/iterations->n_sample;
        }
        MPI_Win_unlock(root, c_info->WIN);
    }
    MPI_Barrier(c_info->communicator);

    *time = res_time; 
    return;
}


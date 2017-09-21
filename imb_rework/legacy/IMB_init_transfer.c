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

 File: IMB_init_transfer.c 

 Implemented functions: 

 IMB_init_transfer;
 IMB_close_transfer;

 ***************************************************************************/





#include "mpi.h"
#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"




/* IMB 3.1 << */
void IMB_init_transfer(struct comm_info* c_info, struct Bench* Bmark, int size, MPI_Aint acc_size)
/* >> IMB 3.1  */
/*

                      
                      For IO  case: file splitting/view is set, file is opened
                      For EXT case: window is created and synchronized (MPI_Win_fence)
                      


Input variables: 

-Bmark                (type struct Bench*)                      
                      (For explanation of struct Bench type:
                      describes all aspects of modes of a benchmark;
                      see [1] for more information)
                      
                      Given benchmark
                      

-size                 (type int)                      
                      (Only IO case): used to determine file view
                      
IMB 3.1 <<
-acc_size             (type int)                      
                      (Only EXT case): accumulate window size
>> IMB 3.1


In/out variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      
                      Corresponding components (File or Window related) are set
                      


*/
{


#if defined MPIIO
    #include <limits.h>	
    int ne, baslen, mod;
    int ierr;
    size_t pos1, pos2;

    if( c_info->File_rank < 0 || Bmark->access == no ) return;

    if( size > 0)
    {
	IMB_get_rank_portion(c_info->File_rank, c_info->all_io_procs, size, asize,
			     &pos1, &pos2);
	baslen =  (pos2 >= pos1) ? pos2-pos1+1 : 0;
    } else
    {
	baslen = 0;
	pos2 = pos1 = 0;
    }

    if( c_info->view != MPI_DATATYPE_NULL )
		    MPI_Type_free(&c_info->view);

    if( Bmark->fpointer == private )
    {

	c_info->split.Locsize	= baslen;
	c_info->split.Offset	= 0;
	c_info->split.Totalsize	= baslen;

	if( Bmark->access == put )
		IMB_set_buf(c_info, c_info->File_rank, 0, 
			    (baslen>0)? baslen-1 : 0 ,
			     1, 0);

	if( Bmark->access == get )
		IMB_set_buf(c_info, c_info->File_rank, 1, 0, 0,
			     (baslen>0)? baslen-1 : 0 );
  
    }

    if( Bmark->fpointer == indv_block || Bmark->fpointer == shared ||
	Bmark->fpointer == explicit )
    {
	int bllen[3];

	MPI_Aint displ[3];
	MPI_Datatype types[3];

	bllen[0]=1; displ[0] = 0; types[0] = MPI_LB;

	bllen[1] = baslen;
	displ[1] = pos1;
	types[1] = c_info->etype;

	bllen[2] = 1;
	displ[2] = size;
	types[2] = MPI_UB;

	if( Bmark->fpointer == indv_block )
	{
	    /* July 2002 fix V2.2.1: handle empty view case separately */
	    if( baslen>0 )
	    {
		/* end change */
		ierr=MPI_Type_struct(3,bllen,displ,types,&c_info->view);
		IMB_err_hand(1,ierr);
		ierr=MPI_Type_commit(&c_info->view);
		IMB_err_hand(1,ierr);
		c_info->filetype = c_info->view;

		/* July 2002 fix V2.2.1: handle empty case */
	    }
	    else c_info->filetype = c_info->etype;
	    /* end change */
	}

	if( Bmark->access == put )
	    IMB_set_buf(c_info, c_info->File_rank, 0, (baslen>0)? baslen-1 : 0, 1, 0 );

	if( Bmark->access == get )
	    IMB_set_buf(c_info, c_info->File_rank, 1, 0, 0, (baslen>0)? baslen-1 : 0 );

	c_info->split.Locsize = bllen[1];
	c_info->split.Offset  = pos1;
	c_info->split.Totalsize = size;
    }

    ierr = IMB_open_file(c_info);

#elif defined  EXT
    MPI_Aint sz;
    int s_size, r_size;
    int ierr;

    ierr=0;

    if( Bmark->reduction )
    {
	MPI_Type_size(c_info->red_data_type,&s_size);
	r_size=s_size;
    }
    else
    {
	MPI_Type_size(c_info->s_data_type,&s_size);
	MPI_Type_size(c_info->r_data_type,&r_size);
    }

    if( c_info -> rank >= 0 )
    {
	IMB_user_set_info(&c_info->info);

	/* IMB 3.1 << */
	sz = acc_size;
	/* >> IMB 3.1  */

	if( Bmark->access == put)
	{
	    ierr = MPI_Win_create(c_info->r_buffer,sz,r_size,c_info->info,
				  c_info->communicator, &c_info->WIN);
	    MPI_ERRHAND(ierr);
	    ierr = MPI_Win_fence(0, c_info->WIN);
	    MPI_ERRHAND(ierr);
	}
	else if( Bmark->access == get)
	{
	    ierr = MPI_Win_create(c_info->s_buffer,sz,s_size,c_info->info,
				  c_info->communicator, &c_info->WIN);
	    MPI_ERRHAND(ierr);
	    ierr = MPI_Win_fence(0, c_info->WIN);
	    MPI_ERRHAND(ierr);
	}
    }
#elif defined RMA
    int s_size, r_size;
    int ierr = 0;

    if(Bmark->reduction)
    {
        MPI_Type_size(c_info->red_data_type,&s_size);
        r_size=s_size;
    }
    else
    {
        MPI_Type_size(c_info->s_data_type,&s_size);
        MPI_Type_size(c_info->r_data_type,&r_size);
    }

    if(c_info->rank >= 0)
    {
        IMB_user_set_info(&c_info->info);

        if (Bmark->access == put)
        {    
            ierr = MPI_Win_create(c_info->r_buffer, acc_size, r_size, c_info->info,
                    c_info->communicator, &c_info->WIN);
        }
        else if (Bmark->access == get)
        {
            ierr = MPI_Win_create(c_info->s_buffer, acc_size, r_size, c_info->info,
                    c_info->communicator, &c_info->WIN);
        }    
        MPI_ERRHAND(ierr);
    }
#endif 

    IMB_set_errhand(c_info);
    err_flag = 0;
}


void IMB_close_transfer (struct comm_info* c_info, struct Bench* Bmark, int size)
    /*


       Closes / frees file / window components



       Input variables: 

       -Bmark                (type struct Bench*)                      
       (For explanation of struct Bench type:
       describes all aspects of modes of a benchmark;
       see [1] for more information)

       Given benchmark


       -size                 (type int)                      
       (Only IO case): used to determine file view



       In/out variables: 

       -c_info               (type struct comm_info*)                      
       Collection of all base data for MPI;
       see [1] for more information

       Corresponding components (File or Window related) are freed



*/
{
#ifdef MPIIO
    if( c_info->view != MPI_DATATYPE_NULL )
        MPI_Type_free(&c_info->view);

    if( c_info->File_rank >= 0 && Bmark->access != no && c_info->fh!=MPI_FILE_NULL)
        MPI_File_close(&c_info->fh);

#else /*not MPIIO*/
#if (defined EXT || defined RMA)

    if( c_info->WIN != MPI_WIN_NULL )
        MPI_Win_free(&c_info->WIN);

#endif /*EXT || RMA*/
#endif /*MPIIO*/
}


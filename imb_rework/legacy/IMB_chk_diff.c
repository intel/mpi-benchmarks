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

 A collection of auxiliary functions ONLY for the CHECK mode of IMB
 ===================================================================

 File: IMB_chk_diff.c 

 Implemented functions: 

 IMB_chk_dadd;
 IMB_ddiff;
 IMB_show;
 IMB_err_msg;
 IMB_chk_diff;
 IMB_cmp_cat;
 IMB_chk_contiguous;
 IMB_chk_distr;
 IMB_chk_contained;
 IMB_compute_crc;

 ***************************************************************************/





#include "IMB_declare.h"
#include "IMB_benchmark.h"


#include "IMB_prototypes.h"

#include <limits.h>


void IMB_chk_dadd(void* AUX, int Locsize, size_t buf_pos, 
                  int rank0, int rank1)
/*

                      
                      Auxiliary, only for checking mode; 
                      Creates reference accumulated values in a given 
                      buffer section, accumulated over given processor ranks 
                      


Input variables: 

-Locsize              (type int)                      
                      Size of buffer section to check
                      

-buf_pos              (type int)                      
                      Start position of buffer section
                      

-rank0                (type int)
-rank1                (type int)                      
                      Process' values between rank0 and rank1 are accumulated
                      


In/out variables: 

-AUX                  (type void*)                      
                      Contains accumulated values
                      


*/
{
/* Sum up all process' data buffers in certain window */
    int  rank;
    size_t i;

    for(i=0; i<Locsize/asize; i++)
	((assign_type*)AUX)[i]  = 0.;

    for(rank = rank0; rank<= rank1; rank++)
    {
	for(i=0; i<Locsize/asize; i++)
	    ((assign_type*)AUX)[i] += BUF_VALUE(rank,buf_pos/asize+i);
    }
}


double IMB_ddiff(assign_type *A, assign_type *B, size_t len, 
                 size_t *fault_pos)
/*

                      
                      Compares the values of 2 buffers A, B and returns max. diff
                      


Input variables: 

-A                    (type assign_type *)                      
                      Buffer of values
                      

-B                    (type assign_type *)                      
                      Another buffer of values to be checked against A
                      

-len                  (type int)                      
                      Length (in assign_type items) of A, B
                      


Output variables: 

-fault_pos            (type int *)                      
                      Position of first non tolerable deviation
                      


Return value          (type double)                      
                      Deviation of A and B
                      


*/
{
/* max. relative difference of vectors A/B */
    double D,d1, rel;
    size_t i;

    D=0.;

    d1 = -1.;
    *fault_pos = CHK_NO_FAULT;  

    if( len > 0 )
    {
	for(i =0; i<len && d1 <= TOL; i++)
	{
	    if( A[i] != 0. ) rel=A_ABS(A[i]);
	    else rel=1.;
 
	    d1 = A_ABS(A[i]-B[i])/rel;

	}
	D = d1;

	if( D  > TOL ) 
	{
	    D=1.;
	    IMB_Assert(i>0);
	    *fault_pos = (i-1)*asize;
	}

    }

    return D;

}


void IMB_show(char* text, struct comm_info* c_info, void* buf, 
              size_t loclen, size_t totlen, int j_sample, 
              POSITIONING fpos)
/*

                      
                      Shows an excerpt of erroneous buffer if occurs in check mode
                      


Input variables: 

-text                 (type char*)                      
                      Accompanying text to put out
                      

-loclen               (type int)                      
                      Local length of buffer
                      

-totlen               (type int)                      
                      Total length of buffer (for gathered or shared access buffers)
                      

-j_sample             (type int)                      
                      Number of sample the error occurred
                      

-fpos                 (type POSITIONING)                      
                      File positionning (if relevant)
                      

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-buf                  (type void*)                      
                      Given check buffer
                      


*/
{
    size_t i;

    fprintf(unit,"Process %d: %s",c_info->rank,text);
    fprintf(unit,"\n");

#ifdef DEBUG
    size_t j;
    fprintf(dbg_file,"Process %d: %s",c_info->rank,text);
    fprintf(dbg_file,"\n");
#endif /*DEBUG*/

#ifdef MPIIO
    {
	MPI_Offset Offset;
	switch(fpos)
	{
	    case indv_block:
		Offset = (MPI_Offset)(j_sample*totlen);
		break;

	    case explicit:
		Offset = c_info->split.Offset+(MPI_Offset)(j_sample*totlen);
		break;

	    case private:
		Offset = (MPI_Offset)(j_sample*loclen);
		break;

	    case shared:
		Offset = (MPI_Offset)(-1);
		break;
	}

	if( fpos == shared )
	    fprintf(unit,
#ifdef WIN_IMB
		    "Overall size = %I64u,"
		    " Portion = %I64u,"
#else
		    "Overall size = %lu,"
		    " Portion = %lu,"
#endif /* WIN_IMB*/
		    " #sample= %d\n",
		    totlen,loclen,j_sample);
	else if( fpos != -1 )
	    fprintf(unit,
#ifdef WIN_IMB
		    "Overall size = %I64u,"
		    " Portion = %I64u,"
#else
		    "Overall size = %lu,"
		    " Portion = %lu,"
#endif /* WIN_IMB*/
		    " Startpos = %ld\n",
		    totlen,loclen,(long)Offset);

#ifdef DEBUG
	if( fpos == shared )
	    fprintf(dbg_file,
#ifdef WIN_IMB
			    "Overall size = %I64u,"
			    " Portion = %I64u,"
#else
			    "Overall size = %lu,"
			    " Portion = %lu,"
#endif /*WIN_IMB*/
			    " #sample= %d\n",
			    totlen,loclen,j_sample);
	else if( fpos != -1 )
	    fprintf(dbg_file,
#ifdef WIN_IMB
			    "Overall size = %I64u,"
			    " Portion = %I64u,"
#else
			    "Overall size = %lu,"
			    " Portion = %lu,"
#endif /*WIN_IMB*/
			    " Startpos = %ld\n",
			    totlen,loclen,(long)Offset);
#endif /*DEBUG*/

    }
#endif /*MPIIO*/

    if( loclen < asize )
    {
	if( loclen == 0 )
	{
	    fprintf(unit,"Buffer empty\n");
#ifdef DEBUG
	    fprintf(dbg_file,"Buffer empty\n");
#endif
	}
	else
	{
	    fprintf(unit,"Buffer in bytewise int representation: ");

	    for (i = 0; i<loclen; i++)
		fprintf(unit,"%d ",((char*)buf)[i]);

	    fprintf(unit,"\n");
#ifdef DEBUG
	    fprintf(dbg_file,"Buffer in bytewise int representation: ");

	    for (i = 0; i<loclen; i++)
		fprintf(dbg_file,"%d ",((char*)buf)[i]);
	    fprintf(dbg_file,"\n");
#endif /*DEBUG*/
	}
    }
    else
    {
	if( loclen >= 2*asize)
	    fprintf(unit,"Buffer, 1st and last entry: ");
	else
	    fprintf(unit,"Buffer entry: ");

#ifdef BUFFERS_INT
	fprintf(unit,"%d ",((assign_type*)buf)[0]);
	if(loclen>=2*asize) fprintf(unit,"%d ",((assign_type*)buf)[loclen/asize-1]);
#endif

#ifdef BUFFERS_FLOAT
	fprintf(unit,"%f ",((assign_type*)buf)[0]);
	if(loclen>=2*asize) fprintf(unit,"%f ",((assign_type*)buf)[loclen/asize-1]);
#endif

	fprintf(unit,"\n");

#ifdef DEBUG
	if(err_flag)
	    for(j=0; j<loclen/asize; j+=5)
	    {
#ifdef BUFFERS_INT
		for(i=j; i<min(loclen/asize,j+5); i++)
		    fprintf(dbg_file,"%d ",((assign_type*)buf)[i]);
#endif /*BUFFERS_INT*/
#ifdef BUFFERS_FLOAT
		for(i=j; i<min(loclen/asize,j+5); i++)
		    fprintf(dbg_file,"%f ",((assign_type*)buf)[i]);
#endif /*BUFFERS_FLOAT*/
		fprintf(dbg_file,"\n");
	    } /*for*/*/
#endif /*DEBUG*/
    }
    fflush(unit);

#ifdef DEBUG
    fflush(dbg_file);
#endif

}

void IMB_err_msg(struct comm_info* c_info, char* text, size_t totsize, 
                 int j_sample)
/*

                      
                      Outputs an brief error diagnostics if occurs
                      


Input variables: 

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-text                 (type char*)                      
                      Accompanying text
                      

-totsize              (type int)                      
                      Size of the erroneous buffer
                      

-j_sample             (type int)                      
                      Number of sample the error occured in
                      


*/
{
    fprintf(unit,"%d: Error %s,"
#ifdef WIN_IMB
	    "size = %I64u,"
#else
	    "size = %lu,"
#endif
	    "sample #%d\n",
	    c_info->rank,text,totsize,j_sample);
}

#ifdef CHECK



void IMB_chk_diff(char* text, struct comm_info* c_info, void* RECEIVED, 
                  size_t buf_pos, int Locsize, size_t Totalsize, 
                  int unit_size, DIRECTION mode, POSITIONING fpos, 
                  int n_sample, int j_sample, int source, 
                  double* diff )
/*

                      
                      Checks a received buffer against expected ref values
                      


Input variables: 

-text                 (type char*)                      
                      Accompanying text
                      

-c_info               (type struct comm_info*)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-RECEIVED             (type void*)                      
                      The buffer to be checked
                      

-buf_pos              (type int)                      
                      Beginning position (in units -> unit_size)
                      

-Locsize              (type int)                      
                      Local buffer size
                      

-Totalsize            (type int)                      
                      Total buffer size (in case of gathered buffers)
                      

-unit_size            (type int)                      
                      Base unit for positioning
                      

-mode                 (type DIRECTION)                      
                      Direction of the action that took place
                      

-fpos                 (type POSITIONING)                      
                      File positioning of the action that took place (if relevant)
                      

-n_sample             (type int)                      
                      # overall samples
                      

-j_sample             (type int)                      
                      current sample
                      

-source               (type int)                      
                      Sending process (if relevant)
                      


Output variables: 

-diff                 (type double*)                      
                      The error against expected values
                      


*/
{
#ifdef MPIIO
    MPI_File	restore;
    MPI_Status	stat;
    double	def_tmp;
    int 	j, j1, j2, ierr, rank, allpos;
    size_t	pos1, pos2;
    int*	rankj;
    size_t*	lenj;
#endif

    double defloc;

    size_t faultpos, pos;

    int    *all_ranks, Npos;
    size_t *lengths;

    if( err_flag ) return;

    defloc = 0.;
    faultpos = CHK_NO_FAULT;

    if (Totalsize == 0) 
    {
	*diff = 0.;
	return;
    }

#ifdef MPIIO

    MPI_Barrier(c_info->File_comm);

    if( mode == put )
    {
	if( c_info -> File_rank == 0 )
	{

	    IMB_alloc_buf(c_info,"Write check",0,Totalsize);

	    ierr = MPI_File_open(MPI_COMM_SELF, c_info->filename,
				 c_info->amode, c_info->info, &restore);
	    IMB_err_hand(1,ierr);

	    if(source == -3 )
	    {
		IMB_i_alloc(size_t, lengths,c_info->File_num_procs*n_sample+n_sample-1,"chk_diff 1");
		IMB_i_alloc(int, all_ranks,c_info->File_num_procs*n_sample+n_sample-1,"chk_diff 2");
	    }
	    else
	    {
		IMB_i_alloc(size_t, lengths,c_info->File_num_procs,"chk_diff 1a");
		IMB_i_alloc(int, all_ranks,c_info->File_num_procs,"chk_diff 2a");
	    }

	    lenj  = lengths;
	    rankj = all_ranks;
	    allpos = 0;

	    if( j_sample < 0 ) 
	    {
		j1 = 0; 
		j2 = n_sample-1;
	    }
	    else
	    {
		j1 = j_sample; 
		j2 = j_sample;
	    }

	    ierr = MPI_File_seek(restore,(MPI_Offset)(j1*Totalsize),MPI_SEEK_SET);
	    MPI_ERRHAND(ierr);

	    for( j=j1; j<=j2 && faultpos==CHK_NO_FAULT /*faultpos<0*/ ; j++ )
	    {
		IMB_Assert(Totalsize<=INT_MAX);

		ierr = MPI_File_read(restore,c_info->r_buffer,
				     (int) Totalsize,c_info->etype,&stat);

		MPI_ERRHAND(ierr);
		RECEIVED = c_info->r_buffer;

		if( source == -3 )
		{
		    IMB_cmp_cat(c_info, RECEIVED, Totalsize, buf_pos, unit_size, 1, 
				lenj, rankj, &Npos, &faultpos, &def_tmp);

		    lenj  += Npos;
		    rankj += Npos;
		    allpos+= Npos;
		}
		else
		{
		    IMB_cmp_cat(c_info, RECEIVED, Totalsize, buf_pos, unit_size, 0, 
				lenj, rankj, &Npos, &faultpos, &def_tmp);
		}

		defloc = max(defloc,def_tmp);

	    } /*for( j=j...*/

	    MPI_File_close(&restore);


	    j_sample = j-1;

	    IMB_free_aux();

	    if( /*faultpos >= 0*/ faultpos != CHK_NO_FAULT ) 
	    {

		IMB_err_msg(c_info,text,Totalsize,j_sample);
		fprintf(unit,
			"Error: restored buffer from output file, invalid portion starting at pos."
#ifdef WIN_IMB
			"  %I64u\n",
#else
			"  %lu\n",
#endif
			(j_sample*Totalsize)+faultpos);

		AUX = (void*)(((char*)RECEIVED)+faultpos);
		IMB_show("Erroneous data:",c_info,AUX,Totalsize-faultpos,Totalsize-faultpos,j_sample,nothing);

	    }
	    else
	    {
		if( source == -3 )
		{
		    IMB_chk_distr(c_info, Totalsize, n_sample, lengths, all_ranks, allpos, &def_tmp);
  
		    if( def_tmp > 0. )
		    {
			IMB_err_msg(c_info,text,Totalsize,j_sample);
			IMB_show("restored buffer from output file, has permuted data: ",
				 c_info,RECEIVED,Totalsize,Totalsize,j_sample,nothing);
		    }
		}

		defloc = max(defloc,def_tmp);

	    } /*if( faultpos >= 0 ) */

	    IMB_del_r_buf(c_info);
	    IMB_v_free((void**)&lengths); IMB_v_free ((void**)&all_ranks);
	} /*if( c_info -> File_rank == 0 )*/

	fflush(unit);
    }   /*if( mode == put )*/

    if( mode == get )
    {
	size_t file_pos;
	file_pos = j_sample*Totalsize;

	*diff=0.;

	IMB_alloc_aux(Totalsize," chk_diff 5");
	IMB_init_file_content(AUX, file_pos, file_pos+Totalsize-1);

	IMB_chk_contained(RECEIVED, Locsize, AUX, Totalsize, &pos, &faultpos, &def_tmp,
			  "Compare received portion with file Content");

	IMB_get_rank_portion(c_info->File_rank,c_info->File_num_procs,Totalsize,asize, 
			     &pos1, &pos2);

	if( /*faultpos >= 0*/ faultpos != CHK_NO_FAULT) /* the type of faultpos is changed to size_t*/
	{
	    err_flag=1;
	    defloc = 1; 
	    IMB_err_msg(c_info,text,Totalsize,j_sample);
	    RECEIVED = (void*)((char*)RECEIVED+faultpos);

	    fprintf(unit,
		    "File position: "
#ifdef WIN_IMB
		    "%I64u\n",
#else
		    "%lu\n",
#endif
		    file_pos+pos+faultpos);

	    IMB_show( "Read invalid portion: ",c_info, RECEIVED,
		      Locsize-faultpos, Totalsize, j_sample, fpos);

	    AUX = (void*)((char*)AUX + pos + faultpos);

	    IMB_show( "Expected portion: ",c_info, AUX,
		      Locsize-pos-faultpos, Locsize-pos-faultpos, j_sample, nothing);

	    MPI_Gather(&pos,1,MPI_UNSIGNED_LONG, c_info->rdispl,1,MPI_INT,0,c_info->File_comm);
	}
	else
	{
	    if( source == -2 && Locsize > 0 )
	    {
		IMB_get_rank_portion(c_info->File_rank,c_info->File_num_procs,Totalsize,asize, 
				     &pos1, &pos2);
		if( pos1 != pos ) defloc = 1; 
	    }
   
	    if( source == -3 )
	    {
		/* Check permuted buffer */

		MPI_Gather(&pos,1,MPI_INT,c_info->rdispl,1,MPI_INT,0,c_info->File_comm);
		MPI_Gather(&Locsize,1,MPI_INT,c_info->reccnt,1,MPI_INT,0,c_info->File_comm);

		if( c_info->File_rank == 0 )
		{
		    IMB_chk_contiguous(c_info, c_info->rdispl, c_info->reccnt, &defloc );
		}
		else defloc=0.;

	    }

	    if( defloc > 0. )
		IMB_err_msg(c_info,"Wrong portion ordering in read buffer",Totalsize,j_sample);
	} /*if( faultpos >= 0 )*/

	defloc = max(defloc,def_tmp);
    } /*if( mode == get )*/

    MPI_Barrier(c_info->File_comm);

#else /*not  MPIIO*/

    if( source >= 0 )
    {
	IMB_alloc_aux(Totalsize,"chk_diff 6");
	IMB_ass_buf(AUX, source, buf_pos, 
		    (buf_pos+Totalsize>0)? buf_pos+Totalsize-1 : 0, 1);

   
	if( Totalsize < asize )
	{
	    IMB_chk_contained(RECEIVED, Totalsize, AUX, Totalsize, &pos, &faultpos, &defloc,
			      "Compare received with expected portion");

	    if( /*faultpos>=0*/ faultpos != CHK_NO_FAULT) /* type of faultpos is changed to size_t*/
	    {
		faultpos = 0; 
		defloc = 1.;
	    }
	}
	else
	    defloc =  IMB_ddiff((assign_type *)AUX, (assign_type *)RECEIVED, Totalsize/asize,&faultpos);

    }
    else if( source == -1 )
    {
	IMB_alloc_aux(Locsize,"chk_diff 7");

	IMB_chk_dadd(AUX,Locsize,buf_pos,0,c_info->num_procs-1);

	defloc = IMB_ddiff((assign_type *)AUX, (assign_type *)RECEIVED, Locsize/asize,&faultpos);
   
    }
    else
    {
	if( source == -2 )
	{
	    lengths = NULL;
	    all_ranks = NULL;

	    IMB_cmp_cat(c_info, RECEIVED, Totalsize, buf_pos, unit_size, 0, 
			lengths, all_ranks, &Npos, &faultpos, &defloc);
	}

    }

    if( /*faultpos>=0*/ faultpos != CHK_NO_FAULT) /* type of faultpos is changed to size_t*/
    {
	void* tmp = (void*)(((char *)RECEIVED)+faultpos);

	IMB_err_msg(c_info,text,Totalsize,j_sample);
	IMB_show( "Got invalid buffer: ",c_info, tmp, asize, asize, j_sample, -1);

	fprintf(unit,
#ifdef WIN_IMB
		    "pos: %I64u\n"
#else
		    "pos: %lu\n"
#endif /*WIN_IMB*/
		    , faultpos);

	tmp = (void*)(((char *)AUX     )+faultpos);

	IMB_show( "Expected    buffer: ",c_info, tmp, asize, asize, j_sample, -1);

	defloc = 1;
    }
    IMB_free_aux();
#endif /*MPIIO*/


    if( defloc > TOL ) 
	err_flag = 1;

    *diff = max(*diff,defloc);

}


void IMB_cmp_cat(struct comm_info *c_info, void* RECEIVED, size_t size, 
                 size_t bufpos, int unit_size, int perm, 
                 size_t* lengths, int*ranks, int* Npos, 
                 size_t *faultpos, double* diff)
/*

                      
                      Checks a received buffer which is a concatenation of 
                      several processes' buffers
                      


Input variables: 

-c_info               (type struct comm_info *)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-RECEIVED             (type void*)                      
                      The buffer to be checked
                      

-size                 (type int)                      
                      Size of the buffer
                      

-bufpos               (type int)                      
                      First position to check (in units -> unit_size)
                      

-unit_size            (type int)                      
                      Base unit of positioning
                      

-perm                 (type int)                      
                      Logical flag: 1 if the different rank's portions
                      are potentially in non natural order (relevant for
                      shared file accesses)
                      


Output variables: 

-lengths              (type int*)                      
                      An array of lengths (of a number of erroneous portions)
                      

-ranks                (type int*)                      
                      An array of ranks (the erroneous portions belong to)
                      

-Npos                 (type int*)                      
                      Numer of erroneous portions found (=size of 'lengths' and 'ranks' arrays)
                      

-faultpos             (type int *)                      
                      Position of first found fault
                      

-diff                 (type double*)                      
                      Diff value
                      


*/
{
    int    rank, NP; 
    size_t pos1, pos2, pos, rsize, rem_size;

    int chk_ok;
    assign_type *a,*r;

    double tmp_diff;

    *diff = 0.;
    *faultpos = CHK_NO_FAULT;

    if( size == 0) return;

#ifdef MPIIO
    NP = c_info->File_num_procs;
#else
    NP = c_info->num_procs;
#endif

    rsize = (size+asize-1)/asize*asize;

    IMB_alloc_aux(rsize, "chk_diff 8");

    r = (assign_type*)AUX;
    a = (assign_type*)RECEIVED;

    chk_ok = 0;

    if(perm)
    {
	*Npos = 0;
	pos = 0;

	/* Check beginning of buffer */

	for( rank=0; rank<NP && !chk_ok; rank++ )
	{
	    if( size > 0)
	    {
		IMB_get_rank_portion(rank, NP, size, unit_size, &pos1, &pos2);

		rsize = pos2-pos1+1;
		IMB_ass_buf(AUX, rank, 0, (rsize>0)? rsize-1 : 0, 1);
	    } else
	    {
		rsize = 0;
		pos2 = pos1 = 0;
	    }

	    
  
	    IMB_chk_contained(RECEIVED,min(asize,rsize),AUX,rsize,
			      &pos,faultpos,&tmp_diff, NULL);

	    /*if(*faultpos < 0 && pos>=0)*/ 
	    /* the type of faultpos and pos is changed to size_t */
	    if( *faultpos == CHK_NO_FAULT)
	    {
		if( rsize <= asize )   chk_ok=1;
		else
		{
		    rem_size = rsize-pos;
		    IMB_chk_contained((void*)(r+pos/asize), rem_size, RECEIVED, rem_size, &pos1,
				      faultpos, &tmp_diff, "Check of first part of received buffer");

		    /*if( *faultpos < 0 && pos1>=0 )*/
		    /* the type of faultpos and pos is changed to size_t */
		    if( *faultpos == CHK_NO_FAULT)
		    {
			lengths[*Npos] = rem_size;
			ranks[*Npos] = rank;
			pos = rem_size;
			(*Npos)++;
			chk_ok=1;
		    }

		} /*if( rsize <= asize )*/
	    } /*if( *faultpos == CHK_NO_FAULT)*/
	} /*for( rank=0...*/

	if( !chk_ok )
	{
	    *faultpos = 0;
	    *diff = 1.;
	}

	while( (pos < size) && chk_ok )
	{
	    chk_ok = 0;

	    for( rank=0; rank<NP && !chk_ok; rank++ )
	    {
		IMB_get_rank_portion(rank, NP, size, unit_size, &pos1, &pos2);

		IMB_Assert(pos2>=pos1);
		rsize = pos2-pos1+1;

		/*if( rsize > 0 )*/  
		//{

		    rem_size = min(rsize,size-pos);

		    IMB_ass_buf(AUX, rank, 0, (rsize>0)? rsize-1 : 0, 1);

		    IMB_chk_contained(AUX,rem_size,(void*)(a+pos/asize),rem_size,&pos1,
				      faultpos,&tmp_diff,NULL);

		    /*if( *faultpos < 0 && pos1 >= 0 ) */
		    /* the type of faultpos and pos is changed to size_t */
		    if( *faultpos == CHK_NO_FAULT)
		    { 
			lengths[*Npos] = rem_size;
			ranks[*Npos] = rank;
			pos = pos+rsize; 
			(*Npos)++;
			chk_ok = 1;
		    } 
		//}   /* end if(rsize>0) */
	    }   /* end for(rank..) */

	    if( !chk_ok )
	    {
		*faultpos = pos;
		*diff = 1;
	    }

	} /* end while */

    } /* end if(perm) */
    else
    {

	size_t curr=0;
	void*  tmp;

	for( rank=0; rank<NP ; rank++ )
	{
	    IMB_get_rank_portion(rank, NP, size, unit_size, &pos1, &pos2);

	    if( pos2>=pos1)
		rsize = pos2-pos1+1;
	    else 
		rsize = 0;

	    tmp = (void*)(((char*)RECEIVED)+curr);

	    IMB_ass_buf(AUX, rank, bufpos, 
			(bufpos+rsize>0) ? bufpos+rsize-1 : 0 , 1);

	    IMB_chk_contained(AUX,rsize,tmp,rsize,&pos1,faultpos, &tmp_diff,"");
	    *diff = max(*diff,tmp_diff);

	    /*if(*faultpos<0 && pos1>= 0 )*/
	    /* the type of faultpos and pos is changed to size_t */
	    if( *faultpos == CHK_NO_FAULT)
		curr+=rsize;
	    else
	    { 
		*faultpos += curr;
		*diff = 1;
		break;
	    }
	} /*for( rank=0*/
    } /* else if(!perm) */
}




void IMB_chk_contiguous(struct comm_info *c_info, int* rdispl, int* sizes, 
                        double*diff)
/*

                      
                      Checks whether arrays of displacements/sizes form a
                      contiguous buffer
                      


Input variables: 

-c_info               (type struct comm_info *)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-rdispl               (type int*)                      
                      Array of displacements (one for each process)
                      

-sizes                (type int*)                      
                      Array of sizes (one for each process)
                      


Output variables: 

-diff                 (type double*)                      
                      0 if contiguous, 1 else
                      


*/
{
    int i, j, NP, rank, p;

#ifdef MPIIO
    NP = c_info->File_num_procs;
#else
    NP = c_info->num_procs;
#endif

    for(i=0; i<NP; i++)
    {
	for(j=i; j<NP; j++)
	    if( rdispl[j] < rdispl[i] )
	    {
		p = rdispl[i];
		rdispl[i] = rdispl[j];
		rdispl[j]=p;
		p = sizes[i];
		sizes[i] = sizes[j];
		sizes[j]=p;
	    }
    }

    p=0;
    *diff = 0.;

    for(rank = 0; rank<NP; rank++)
    {
	if( rdispl[rank] == p || sizes[rank] == 0 )
	    p = p+sizes[rank];
	else
	    *diff = 1.; 
    }

    if( *diff > TOL )
    {
	fprintf(unit,"check of contiguity of received buffer portions failed\n");
	fprintf(unit,"Got the following portions/displacements:\n");

	for(rank = 0; rank<NP; rank++)
	{
	    fprintf(unit,"%d / %d; ",sizes[rank], rdispl[rank]);
	}
	fprintf(unit,"\n");
    }

}
               



void IMB_chk_distr(struct comm_info *c_info, size_t size, int n_sample, 
                   size_t* lengths, int* ranks, int Npos, 
                   double *diff)
/*

                      
                      (Only for MPI-IO shared file pointer accesses)
                      Checks whether a found set of section lengths/ranks in
                      a file meets expectations
                      


Input variables: 

-c_info               (type struct comm_info *)                      
                      Collection of all base data for MPI;
                      see [1] for more information
                      

-size                 (type int)                      
                      Size of buffer
                      

-n_sample             (type int)                      
                      Number of samples expected in file
                      

-lengths              (type int*)                      
                      Array of section lengths found
                      

-ranks                (type int*)                      
                      Array of ranks belonging to sections
                      

-Npos                 (type int)                      
                      Number of sections
                      


Output variables: 

-diff                 (type double *)                      
                      0 if set is consistent, 1 else
                      


*/
{
    int i, NP, rank;
    size_t pos1, pos2;

    *diff = 0.;

    if( size == 0) return;

#ifdef MPIIO
    NP = c_info->File_num_procs;
#else
    NP = c_info->num_procs;
#endif

    for( rank=0; rank<NP; rank++ )
    {
	c_info->reccnt[rank]=0;
    }

    i=0;
    while (i<Npos)
    {
	rank = ranks[i];

	IMB_get_rank_portion(rank, NP, size, asize, &pos1, &pos2);

	if( pos2-pos1+1 == lengths[i] ) 
	{
	    c_info->reccnt[rank]++;
	}
	else if( i<Npos )
	{
	    if( ranks[i+1] == rank && pos2-pos1+1 == (lengths[i]+(lengths[i+1])) )
	    {
		c_info->reccnt[rank]++; i++;
	    }
	}

	i++;
    } /*while*/

    for( rank=0; rank<NP; rank++ )
    {
	IMB_get_rank_portion(rank, NP, size, asize, &pos1, &pos2);
	if( pos2>= pos1 && c_info->reccnt[rank] != n_sample ) *diff = 1; 
    }

    if( *diff > TOL )
    {
	fprintf(unit,"check of contiguity of received buffer portions failed\n");
	fprintf(unit,"Got the following portions/from process:\n");

	for(i=0; i<Npos; i++)
	{
	    fprintf(unit,
#ifdef WIN_IMB
		    "%I64u / %d; ",
#else
		    "%lu / %d; ",
#endif
		    lengths[i], ranks[i]);
	}
	fprintf(unit,"\n");
    }

}




void IMB_chk_contained(void* part, size_t p_size, void* whole, 
                       size_t w_size, size_t* pos, size_t* fpos, 
                       double* D, char*msg)
/*

                      
                      Checks whether a buffer part is contained in a larger buffer
                      (exploits uniqueness of buffer values, so check is trivial)
                      


Input variables: 

-part                 (type void*)                      
                      Partial buffer
                      

-p_size               (type int)                      
                      Size of partial buffer
                      

-whole                (type void*)                      
                      Whole buffer
                      

-w_size               (type int)                      
                      Size of whole buffer
                      

-msg                  (type char*)                      
                      Accompanying message
                      


Output variables: 

-pos                  (type int*)                      
                      Position where partial buffer begins in whole buffer
                      if search was successful
                      

-fpos                 (type int*)                      
                      Position where first fault occurred when start position was
                      found, but later an error occurred
                      

-D                    (type double*)                      
                      0 if check positive, 1 else
                      


*/
{
    assign_type *a_part, *a_whole;
    long pcrc, wcrc;
    size_t w_len, p_len;

    a_part = (assign_type*) part;
    a_whole = (assign_type*) whole;

    *fpos = CHK_NO_FAULT;  /* instead of -1*/
    *D=0.;

    if( /*p_size <= 0*/ p_size == 0) /*!!! the type of p_size is changed to unsigned size_t*/
	*pos = 0;
    else if ( p_size > w_size )
    {
	*pos = 0; *fpos = 0;
    }
    else
    {

	if( p_size < asize )
	{
	    pcrc = IMB_compute_crc ((char*)part, p_size);

	    *pos = 0;
	    wcrc = pcrc-1;

	    while( *pos <= w_size-p_size && wcrc != pcrc )
	    {
		void* h;
		h = (void*)(((char*)whole)+*pos); 
		wcrc = IMB_compute_crc ((char*)h, p_size);

		if(wcrc!=pcrc) (*pos)++;
	    }

	    if( *pos <= w_size-p_size ) *D=0.;
	    else {
		*pos=0; 
		*fpos = 0; 
		*D=1.;
	    }
	} /*if( p_size < asize )*/
	else
	{
	    *pos = 0;
	    w_len = w_size/asize;
	    p_len = p_size/asize;

	    while( *pos <= w_len-p_len && A_ABS(a_part[0] - a_whole[*pos]) > TOL )  
		    (*pos)++;

	    if( *pos <= w_len-p_len )
	    {
		*D = IMB_ddiff(a_part, a_whole+*pos, p_len, fpos);
	    }
	    else 
	    {
		*D=1.; 
		*pos=0; 
		*fpos = 0; 
	    }

	    *pos *= asize;
	} /*if !( p_size < asize )*/
    }

    if( *fpos != CHK_NO_FAULT /*>= 0*/ ) *D=1.;
    
}



/********************* CHECKSUM ********************/

/* Most of following CRC-32 stuff is from zmodem source code */

/* I claim no copyright over the contents of this file.  -- Rahul Dhesi */

/*
Checksum:  951252172      (check or update this with "brik")
*/
#define INITCRC 0xFFFFFFFFL
/*
 * Copyright (C) 1986 Gary S. Brown.  You may use this program, or
 * code or tables extracted from it, as desired without restriction.
 */

/* First, the polynomial itself and its table of feedback terms.  The  */
/* polynomial is                                                       */
/* X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0 */
/* Note that we take it "backwards" and put the highest-order term in  */
/* the lowest-order bit.  The X^32 term is "implied"; the LSB is the   */
/* X^31 term, etc.  The X^0 term (usually shown as "+1") results in    */
/* the MSB being 1.                                                    */

/* Note that the usual hardware shift register implementation, which   */
/* is what we're using (we're merely optimizing it by doing eight-bit  */
/* chunks at a time) shifts bits into the lowest-order term.  In our   */
/* implementation, that means shifting towards the right.  Why do we   */
/* do it this way?  Because the calculated CRC must be transmitted in  */
/* order from highest-order term to lowest-order term.  UARTs transmit */
/* characters in order from LSB to MSB.  By storing the CRC this way,  */
/* we hand it to the UART in the order low-byte to high-byte; the UART */
/* sends each low-bit to hight-bit; and the result is transmission bit */
/* by bit from highest- to lowest-order term without requiring any bit */
/* shuffling on our part.  Reception works similarly.                  */

/* The feedback terms table consists of 256, 32-bit entries.  Notes:   */
/*                                                                     */
/*     The table can be generated at runtime if desired; code to do so */
/*     is shown later.  It might not be obvious, but the feedback      */
/*     terms simply represent the results of eight shift/xor opera-    */
/*     tions for all combinations of data and CRC register values.     */
/*                                                                     */
/*     The values must be right-shifted by eight bits by the "updcrc"  */
/*     logic; the shift must be unsigned (bring in zeroes).  On some   */
/*     hardware you could probably optimize the shift in assembler by  */
/*     using byte-swap instructions.                                   */


static long crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
      0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
      0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
      0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
      0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
      0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
      0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
      0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
      0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
      0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
      0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
      0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
      0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
      0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
      0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
      0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
      0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
      0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
      0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
      0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
      0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
      0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
      0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
      0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
      0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
      0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
      0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
      0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
      0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
      0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
      0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
      0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
      0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
      0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
      0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
      0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
      0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
      0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
      0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
      0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
      0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
      0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
      0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
      0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
      0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
      0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
      0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
      0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
      0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
      0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
      0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
      0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
      0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
      0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
      0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
      0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
      0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
      0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
      0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
      0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
      0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
      0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
      0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
      0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
      0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};



long IMB_compute_crc (register char* buf, register size_t size)
/*



In/out variables: 

-buf                  (type register char*)
-size                 (type register int)

Return value          (type long)

*/
{
    long crccode = INITCRC;

    if( /*size <= 0*/ size == 0 )   /*!!! the type of size is modified to unsigned size_t*/
	crccode = 0;
    else 
    {
	int i;
	for (i = 0;  i < size;  i ++) 
	{
	    crccode = crc_32_tab[(int) ((crccode) ^ (buf[i])) & 0xff] ^
		(((crccode) >> 8) & 0x00FFFFFFL);
	}
    }

    return(crccode);
}

#endif

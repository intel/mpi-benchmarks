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
    
 File: IMB_strgs.c 

 Implemented functions: 

 IMB_str;
 IMB_lwr;
 IMB_str_atoi;
 IMB_str_erase;

 ***************************************************************************/





#include <string.h>

#include "IMB_declare.h"

#include "IMB_prototypes.h"




char* IMB_str(const char* Bname)
/*

                      
                      Copies string Bname to new memory which is returned
                      


Input variables: 

-Bname                (type char*)                      
                      String to be copied
                      


Return value          (type char*)                      
                      Copy of Bname with newly allocated memory
                      


*/
{
    char* strg = (char*) IMB_v_alloc( 1+strlen(Bname),"str" );

    if(strg)
	return strcpy(strg,Bname);
    else 
	return NULL;
}


/***************************************************************************/
void IMB_lwr(char* Bname)
/*



In/out variables: 

-Bname                (type char*)                      
                      Uper case alphabetic characters are converted to lower case
                      


*/
{
    int i;

    for(i=0; i<strlen(Bname); i++)
	if ( Bname[i] >= 'A' && Bname[i] <= 'Z' )
	    Bname[i] = Bname[i] - ('A'-'a');
}



/***************************************************************************/



int IMB_str_atoi(char s[])
/*

                      
                      Evaluates int value of a numeric string
                      


Input variables: 

-s                    (type char [])                      
                      String with only numeric characters
                      


Return value          (type int)                      
                      Numeric value
                      


*/
{
    int i,n,len ;

    n=0;
    len=strlen(s);

    for(i=0; s[i] >= '0' && s[i] <= '9' || s[i]==' '; ++i)
    {
	if (s[i]!=' ') 
	{
	    n=10*n+(s[i]-'0');
	}
    }
/* IMB_3.0 */
    if (len==0 || i<len)
    {
	n=-1;
    }
    return n;
}


/* str_erase   */
void IMB_str_erase(char* string, int Nblnc)
/*

                      
                      Fills blancs into a string
                      


Input variables: 

-Nblnc                (type int)                      
                      #blancs to fill
                      


In/out variables: 

-string               (type char*)                      
                      Null terminated string with Nblnc many blancs
                      


*/
{
    if( Nblnc > 0 )
    {
	int i;
	for(i=0;i<Nblnc; i++ ) string[i]=' ';

	i=max(0,Nblnc);
	string[i]='\0';
    }
}


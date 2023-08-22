/****************************************************************************
*                                                                           *
* Copyright (C) 2023 Intel Corporation                                      *
*                                                                           *
*****************************************************************************

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ***************************************************************************

For more documentation than found here, see

[1] doc/ReadMe_IMB.txt 

[2] Intel(R) MPI Benchmarks
    Users Guide and Methodology Description
    In 
    doc/IMB_Users_Guide.pdf
    
 File: IMB_g_info.c 

 Implemented functions: 

 IMB_general_info;
 IMB_make_sys_info;
 IMB_end_msg;

 ***************************************************************************/




#ifdef IMB2018
char* VERSION="2018";
#else
char* VERSION="2021.7";
#endif

#include <stdio.h>
#include <time.h>

#include "IMB_declare.h"
#include "IMB_benchmark.h"

#include "IMB_prototypes.h"

extern FILE* unit;




void IMB_general_info() {
/*

                      Prints to stdout some basic information
                      (Version, time, system (see 'IMB_make_sys_info'))

*/
    /*void IMB_make_sys_info();*/
    time_t T;
    struct tm *local_time;

    time(&T);
    fprintf(unit, "#----------------------------------------------------------------\n");

#ifdef MPI1
#ifdef GPU_ENABLE
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, MPI-1 part (GPU)\n", VERSION);
#else
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, MPI-1 part\n", VERSION);
#endif //GPU_ENABLE
#elif defined EXT
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, MPI-2 part\n", VERSION);
#elif defined MPIIO
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, MPI-IO partn", VERSION);
#elif defined NBC
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, MPI-NBC part\n", VERSION);
#elif defined RMA
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, MPI-RMA part\n", VERSION);
#endif


    fprintf(unit, "#----------------------------------------------------------------\n");
    local_time = localtime(&T);
    if (local_time == NULL) exit(1);
    fprintf(unit, "# Date                  : %s", asctime(local_time));

    IMB_make_sys_info();
    fprintf(unit, "\n");
}

/* IMB 3.1 << */
/* include WIN case */
#ifndef WIN_IMB
#include <sys/utsname.h>
#else
#include <Windows.h>
#define INFO_BUFFER_SIZE 32767
#endif
/* >> IMB 3.1  */


void IMB_make_sys_info() {
/*

                      Prints to stdout some basic information about the system
                      (outcome of the 'uname' command)

*/
    int mpi_subversion, mpi_version;
    /* IMB 3.1 << */
#ifndef WIN_IMB
    struct utsname info;
    uname(&info);
    MPI_Get_version(&mpi_version, &mpi_subversion);

    fprintf(unit, "# Machine               : %s\n", info.machine);
    fprintf(unit, "# System                : %s\n", info.sysname);
    fprintf(unit, "# Release               : %s\n", info.release);
    fprintf(unit, "# Version               : %s\n", info.version);
#else
    /* include WIN case */
    OSVERSIONINFOEX info;
    TCHAR infoBuf[INFO_BUFFER_SIZE];
    DWORD bufCharCount = INFO_BUFFER_SIZE;
    char *substr_ptr;

    MPI_Get_version(&mpi_version, &mpi_subversion);

    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO *)&info);

    bufCharCount = ExpandEnvironmentStrings("%PROCESSOR_IDENTIFIER%", infoBuf, INFO_BUFFER_SIZE);

    /* Replace  "Intel64" by "Intel(R) 64" */
    substr_ptr = strstr(infoBuf, "Intel64");
    if (substr_ptr != NULL)
        fprintf(unit, "# Machine               : Intel(R) 64%s\n", substr_ptr + strlen("Intel64"));
    else {
        /* Replace  "EM64T" by "Intel(R) 64" */
        substr_ptr = strstr(infoBuf, "EM64T");
        if (substr_ptr != NULL)
            fprintf(unit, "# Machine               : Intel(R) 64%s\n", substr_ptr + strlen("EM64T"));
        else
            fprintf(unit, "# Machine               : %s\n", infoBuf);
    }

    if (info.dwMajorVersion == 4)
        switch (info.dwMinorVersion) {
            case 90:
                fprintf(unit, "# System                : Windows Me\n");
                break;
            case 10:
                fprintf(unit, "# System                : Windows 98\n");
                break;
            case 0:
                fprintf(unit, "# System                : Windows NT 4.0\n");
                break;
            default:
                break;
    }
    else if (info.dwMajorVersion == 5)
        switch (info.dwMinorVersion) {
            case 2:
                fprintf(unit, "# System                : Windows 2003\n");
                break;
            case 1:
                fprintf(unit, "# System                : Windows XP\n");
                break;
            case 0:
                fprintf(unit, "# System                : Windows 2000\n");
                break;
            default:
                break;
    }
    else if (info.dwMajorVersion == 6)
        switch (info.dwMinorVersion) {
            case 0:
                if (info.wProductType == VER_NT_WORKSTATION)
                    fprintf(unit, "# System                : Windows Vista\n");
                else
                    fprintf(unit, "# System                : Windows Server 2008\n");
                break;
            default:
                break;
    }

    fprintf(unit, "# Release               : %-d.%-d.%-d\n", info.dwMajorVersion,
            info.dwMinorVersion, info.dwBuildNumber);
    fprintf(unit, "# Version               : %s\n", info.szCSDVersion);
#endif
    /* >> IMB 3.1  */
    fprintf(unit, "# MPI Version           : %-d.%-d\n", mpi_version, mpi_subversion);
    fprintf(unit, "# MPI Thread Environment: ");

#ifdef USE_MPI_INIT_THREAD
    switch (mpi_thread_environment)
    {
        case MPI_THREAD_SINGLE:
            fprintf(unit, "MPI_THREAD_SINGLE\n");
            break;

        case MPI_THREAD_FUNNELED:
            fprintf(unit, "MPI_THREAD_FUNNELED\n");
            break;

        case MPI_THREAD_SERIALIZED:
            fprintf(unit, "MPI_THREAD_SERIALIZED\n");
            break;

        default:
            fprintf(unit, "MPI_THREAD_MULTIPLE\n");
            break;
    }
#endif

    // IMB 3.2 add on: Version information to stdout
    if (strcmp(VERSION, "3.2") > 0) {
        fprintf(unit, "\n\n# New default behavior from Version 3.2 on:\n\n");
        fprintf(unit, "\
                      # the number of iterations per message size is cut down \n\
                      # dynamically when a certain run time (per message size sample) \n\
                      # is expected to be exceeded. Time limit is defined by variable \n\
                      # \"SECS_PER_SAMPLE\" (=> IMB_settings.h) \n\
                      # or through the flag => -time \n\
                        ");
    }
}

void IMB_end_msg(struct comm_info* c_info) {
/*

                      Prints to stdout an eventual end message (currently empty)

Input variables:

-c_info               (type struct comm_info*)
                      Collection of all base data for MPI;
                      see [1] for more information

*/
    FILE* u;

    if (c_info) {
        if (c_info->w_rank == 0) {
            for (u = stdout; u; u = (u == unit ? NULL : unit))
                fprintf(u, "\n\n# All processes entering MPI_Finalize\n\n");
        }
    } else
        fprintf(stderr, "\n\n# IMB has MPI_Finalize-d\n\n");
}

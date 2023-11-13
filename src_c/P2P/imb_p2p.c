
/****************************************************************************
*                                                                           *
* Copyright (C) 2020 Intel Corporation                                      *
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

 ***************************************************************************/

#include "imb_p2p.h"

#ifndef WIN_IMB
#include <sys/utsname.h>
#else
#include <Windows.h>
#define INFO_BUFFER_SIZE 32767
#endif

static const char * VERSION = "2021.7";
FILE* unit = NULL;
imb_p2p_configuration_t imb_p2p_config = { 0 };

static void print_main_header(int argc, char **argv);
static void print_main_footer();
static void loading(int argc, char **argv);
static void initialization(int argc, char **argv);
static void finalization();

int main(int argc, char **argv) {
    size_t i;
    loading(argc, argv);
    initialization(argc, argv);
    for (i = 0; i < imb_p2p_config.benchmarks.length; i++) {
        imb_p2p_config.benchmarks.array[i].run();
    }
    finalization();
    return 0;
}

void* imb_p2p_alloc_mem(size_t size) {
    void *ptr = NULL;
    int mpi_error = MPI_Alloc_mem(size, MPI_INFO_NULL, (void *)&ptr);
    if ((mpi_error != MPI_SUCCESS) || (ptr == NULL)) {
        fprintf(unit, "[%d] MPI_Alloc_mem(%" PRIu64 ") error %d, ptr=%p\n",
                imb_p2p_config.rank, size, mpi_error, ptr);
        fflush(unit);
        exit(10);
    }
    return ptr;
}

void imb_p2p_free_mem(void *ptr) {
    int mpi_error = MPI_Free_mem(ptr);
    if (mpi_error != MPI_SUCCESS) {
        fprintf(unit, "[%d] MPI_Free_mem(%p) error %d\n",
                imb_p2p_config.rank, ptr, mpi_error);
        fflush(unit);
        exit(11);
    }
}

void* imb_p2p_realloc_mem(void *old_ptr, size_t old_size, size_t new_size) {
    void * new_ptr;
    if (old_ptr == NULL) {
        return imb_p2p_alloc_mem(new_size);
    }
    if (new_size == 0) {
        imb_p2p_free_mem(old_ptr);
        return NULL;
    }
    if (new_size <= old_size) {
        return old_ptr;
    }
    new_ptr = imb_p2p_alloc_mem(new_size);
    memcpy(new_ptr, old_ptr, old_size);
    imb_p2p_free_mem(old_ptr);
    return new_ptr;
}

void imb_p2p_print_benchmark_header(const char * name) {
    fprintf(unit, "\n");
    fprintf(unit, "#----------------------------------------------------------------\n");
    fprintf(unit, "# Benchmarking %s\n", name);
    fprintf(unit, "# #processes = %d\n", imb_p2p_config.nranks);
    fprintf(unit, "#----------------------------------------------------------------\n");
    fflush(unit);
}

void imb_p2p_barrier(MPI_Comm comm) {
    int size = 0;
    int rank = 0;
    int mask = 0x1;
    int dst, src;
    int tmp = 0;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    for (; mask < size; mask <<= 1) {
        dst = (rank + mask) % size;
        src = (rank - mask + size) % size;
        MPI_Sendrecv(&tmp, 0, MPI_BYTE, dst, IMB_P2P_BARRIER_TAG,
                     &tmp, 0, MPI_BYTE, src, IMB_P2P_BARRIER_TAG,
                     comm, MPI_STATUS_IGNORE);
    }
}

void imb_p2p_pause() {
#ifndef WIN_IMB
    if (imb_p2p_config.pause_usec >= 0) {
        usleep(imb_p2p_config.pause_usec);
    }
#endif
}

static void print_title() {
    fprintf(unit, "#------------------------------------------------------------\n");
    fprintf(unit, "#    Intel(R) MPI Benchmarks %s, IMB-P2P part\n", VERSION);
    fprintf(unit, "#------------------------------------------------------------\n");
}

static void print_main_header(int argc, char **argv) {
    size_t j;
    int i, mpi_subversion, mpi_version;
    time_t t;
    struct tm *local_time;
#ifndef WIN_IMB
    struct utsname info;
#endif
    time(&t);
    print_title();
    local_time = localtime(&t);
    if (local_time == NULL) exit(1);
    fprintf(unit, "# Date                  : %s", asctime(local_time));
#ifndef WIN_IMB
    uname(&info);
    MPI_Get_version(&mpi_version, &mpi_subversion);

    fprintf(unit, "# Machine               : %s\n", info.machine);
    fprintf(unit, "# System                : %s\n", info.sysname);
    fprintf(unit, "# Release               : %s\n", info.release);
    fprintf(unit, "# Version               : %s\n", info.version);
#else /* Windows */
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
    if (substr_ptr != NULL) {
        fprintf(unit, "# Machine               : Intel(R) 64%s\n", substr_ptr + strlen("Intel64"));
    } else {
        /* Replace  "EM64T" by "Intel(R) 64" */
        substr_ptr = strstr(infoBuf, "EM64T");
        if (substr_ptr != NULL) {
            fprintf(unit, "# Machine               : Intel(R) 64%s\n", substr_ptr + strlen("EM64T"));
        } else {
            fprintf(unit, "# Machine               : %s\n", infoBuf);
        }
    }
    if (info.dwMajorVersion == 4) {
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
    } else if (info.dwMajorVersion == 5) {
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
    } else if (info.dwMajorVersion == 6) {
        switch (info.dwMinorVersion) {
        case 0:
            if (info.wProductType == VER_NT_WORKSTATION) {
                fprintf(unit, "# System                : Windows Vista\n");
            } else {
                fprintf(unit, "# System                : Windows Server 2008\n");
            }
            break;
        default:
            break;
        }
        fprintf(unit, "# Release               : %-d.%-d.%-d\n",
                info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber);
        fprintf(unit, "# Version               : %s\n", info.szCSDVersion);
    }
#endif /* Windows */
    fprintf(unit, "# MPI Version           : %-d.%-d\n", mpi_version, mpi_subversion);
    fprintf(unit, "# MPI Thread Environment:\n");
    fprintf(unit, "#\n");
    fprintf(unit, "# Calling sequence was:\n");
    fprintf(unit, "#\n");
    fprintf(unit, "#");
    for (i = 0; i < argc; i++) {
        fprintf(unit, " %s", argv[i]);
    }
    fprintf(unit, "\n");
    fprintf(unit, "#\n");
    fprintf(unit, "# List of Benchmarks to run:\n");
    fprintf(unit, "#\n");
    for (j = 0; j < imb_p2p_config.benchmarks.length; j++) {
        fprintf(unit, "# %s\n", imb_p2p_config.benchmarks.array[j].name);
    }
    fprintf(unit, "#\n");
    fprintf(unit, "# Minimum message length in bytes:   %" PRIu64 "\n", imb_p2p_config.messages.min_size);
    fprintf(unit, "# Maximum message length in bytes:   %" PRIu64 "\n", imb_p2p_config.messages.max_size);
    fprintf(unit, "#\n");
    fflush(unit);
}

static void print_main_footer() {
    fprintf(unit, "\n\n# All processes entering MPI_Finalize\n\n");
    fflush(unit);
    if (unit != stdout) {
        fprintf(stdout, "\n\n# All processes entering MPI_Finalize\n\n");
        fflush(stdout);
    }
}

static void free_benchmarks() {
    if (imb_p2p_config.benchmarks.array) {
        imb_p2p_free_mem(imb_p2p_config.benchmarks.array);
    }
    imb_p2p_config.benchmarks.array = NULL;
    imb_p2p_config.benchmarks.length = 0;
    imb_p2p_config.benchmarks.capacity = 0;
}

static void add_benchmark(const char * name, imb_p2p_procedure_t procedure) {
    if (!imb_p2p_config.benchmarks.array) {
        imb_p2p_config.benchmarks.length = 0;
        imb_p2p_config.benchmarks.capacity = 256;
        imb_p2p_config.benchmarks.array = (imb_p2p_benchmark_t *)imb_p2p_alloc_mem(sizeof(imb_p2p_benchmark_t) * imb_p2p_config.benchmarks.capacity);
    }
    if (imb_p2p_config.benchmarks.length == imb_p2p_config.benchmarks.capacity) {
        imb_p2p_config.benchmarks.array = (imb_p2p_benchmark_t *)imb_p2p_realloc_mem(imb_p2p_config.benchmarks.array,
                                                                                     sizeof(imb_p2p_benchmark_t) * imb_p2p_config.benchmarks.capacity,
                                                                                     sizeof(imb_p2p_benchmark_t) * (imb_p2p_config.benchmarks.capacity * 2));
        imb_p2p_config.benchmarks.capacity *= 2;
    }
    if (!imb_p2p_config.benchmarks.array) exit(1);
    imb_p2p_config.benchmarks.array[imb_p2p_config.benchmarks.length].name = name;
    imb_p2p_config.benchmarks.array[imb_p2p_config.benchmarks.length].run = procedure;
    imb_p2p_config.benchmarks.length++;
}

static void free_messages() {
    if (imb_p2p_config.messages.array) {
        imb_p2p_free_mem(imb_p2p_config.messages.array);
    }
    imb_p2p_config.messages.array = NULL;
    imb_p2p_config.messages.length = 0;
    imb_p2p_config.messages.capacity = 0;
    imb_p2p_config.messages.min_size = 0;
    imb_p2p_config.messages.max_size = 0;
}

static void add_message(size_t size) {
    if (!imb_p2p_config.messages.array) {
        imb_p2p_config.messages.min_size = ((size_t)1) << IMB_P2P_MAX_MSG_LOG;
        imb_p2p_config.messages.max_size = 0;
        imb_p2p_config.messages.length = 0;
        imb_p2p_config.messages.capacity = 512;
        imb_p2p_config.messages.array = (size_t *)imb_p2p_alloc_mem(sizeof(size_t) * imb_p2p_config.messages.capacity);
    }
    if (imb_p2p_config.messages.length == imb_p2p_config.messages.capacity) {
        imb_p2p_config.messages.array = (size_t *)imb_p2p_realloc_mem(imb_p2p_config.messages.array,
                                                                      sizeof(size_t) * imb_p2p_config.messages.capacity,
                                                                      sizeof(size_t) * (imb_p2p_config.messages.capacity * 2));
        imb_p2p_config.messages.capacity *= 2;
    }
    if (!imb_p2p_config.messages.array) exit(1);
    imb_p2p_config.messages.array[imb_p2p_config.messages.length] = size;
    imb_p2p_config.messages.length++;
    if (imb_p2p_config.messages.min_size > size) {
        imb_p2p_config.messages.min_size = size;
    }
    if (imb_p2p_config.messages.max_size < size) {
        imb_p2p_config.messages.max_size = size;
    }
}

static int is_false(const char * s) {
    return !STRCASECMP(s, "0") || !STRCASECMP(s, "off") || !STRCASECMP(s, "disable") || !STRCASECMP(s, "no") || !STRCASECMP(s, "false");
}

static int is_true(const char * s) {
    return !STRCASECMP(s, "1") || !STRCASECMP(s, "on") || !STRCASECMP(s, "enable") || !STRCASECMP(s, "yes") || !STRCASECMP(s, "true");
}

static void initialization(int argc, char **argv) {
    int mpi_error, i;
    mpi_error = MPI_Init(&argc, &argv);
    if (mpi_error != MPI_SUCCESS) {
        fprintf(unit, "MPI_Init() error %d\n", mpi_error);
        fflush(unit);
        exit(1);
    }
    mpi_error = MPI_Comm_rank(MPI_COMM_WORLD, &imb_p2p_config.rank);
    if (mpi_error != MPI_SUCCESS) {
        fprintf(unit, "MPI_Comm_rank() error %d\n", mpi_error);
        fflush(unit);
        exit(2);
    }
    mpi_error = MPI_Comm_size(MPI_COMM_WORLD, &imb_p2p_config.nranks);
    if (mpi_error != MPI_SUCCESS) {
        fprintf(unit, "[%d] MPI_Comm_size() error %d\n", imb_p2p_config.rank, mpi_error);
        fflush(unit);
        exit(3);
    }
    for (i = 0; i < argc; i++) {
        if (!STRCASECMP(argv[i], IMB_P2P_PINGPONG)) {
            add_benchmark(IMB_P2P_PINGPONG, imb_p2p_pingpong);
        } else if (!STRCASECMP(argv[i], IMB_P2P_PINGPING)) {
            add_benchmark(IMB_P2P_PINGPING, imb_p2p_pingping);
        } else if (!STRCASECMP(argv[i], IMB_P2P_UNIRANDOM)) {
            add_benchmark(IMB_P2P_UNIRANDOM, imb_p2p_unirandom);
        } else if (!STRCASECMP(argv[i], IMB_P2P_BIRANDOM)) {
            add_benchmark(IMB_P2P_BIRANDOM, imb_p2p_birandom);
        } else if (!STRCASECMP(argv[i], IMB_P2P_CORANDOM)) {
            add_benchmark(IMB_P2P_CORANDOM, imb_p2p_corandom);
        } else if (!STRCASECMP(argv[i], IMB_P2P_STENCIL2D)) {
            add_benchmark(IMB_P2P_STENCIL2D, imb_p2p_stencil2d);
        } else if (!STRCASECMP(argv[i], IMB_P2P_STENCIL3D)) {
            add_benchmark(IMB_P2P_STENCIL3D, imb_p2p_stencil3d);
        } else if (!STRCASECMP(argv[i], IMB_P2P_SENDRECV_REPLACE)) {
            add_benchmark(IMB_P2P_SENDRECV_REPLACE, imb_p2p_sendrecv_replace);
        } else if (!STRCASECMP(argv[i], "-msgwr")) {
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-msgwr\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                if (is_true(argv[i])) {
                    imb_p2p_config.payload.is_touch_send_buff = 1;
                } else if (is_false(argv[i])) {
                    imb_p2p_config.payload.is_touch_send_buff = 0;
                } else {
                    if (imb_p2p_config.rank == 0) {
                        fprintf(unit, "Unexpected -msgwr argument value. "
                                "Expected: 0, 1, false, true, disable, enable, off, on, no, yes\n");
                        fflush(unit);
                    }
                    exit(1);
                }
            }
        } else if (!STRCASECMP(argv[i], "-msgrd")) {
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-msgrd\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                if (is_true(argv[i])) {
                    imb_p2p_config.payload.is_touch_recv_buff = 1;
                } else if (is_false(argv[i])) {
                    imb_p2p_config.payload.is_touch_recv_buff = 0;
                } else {
                    if (imb_p2p_config.rank == 0) {
                        fprintf(unit, "Unexpected -msgrd argument value. "
                                "Expected: 0, 1, false, true, disable, enable, off, on, no, yes\n");
                        fflush(unit);
                    }
                    exit(1);
                }
            }
        } else if (!STRCASECMP(argv[i], "-pause")) {
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-pause\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                int d = 0;
                int n = sscanf(argv[i], "%d", &d);
                if (n == 1) {
                    imb_p2p_config.pause_usec = d;
                } else {
                    if (imb_p2p_config.rank == 0) {
                        fprintf(unit, "Unexpected format after \"-pause\"\n");
                        fflush(unit);
                    }
                    exit(1);
                }
            }
        } else if (!STRCASECMP(argv[i], "-msgsz")) {
            char s[256];
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-msgsz\"\n");
                    fflush(unit);
                }
                exit(1);
            } else if (strlen(argv[i]) > sizeof(s)) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Unexpected format after \"-msgsz\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                int d = 0;
                int n = sscanf(argv[i], "%d%s", &d, &s[0]);
                size_t size = d;
                if ((n <= 0) || (n == EOF) || (d < 0)) {
                    n = -1;
                } else if (n == 2) {
                    if ((s[0] == 'k') || (s[0] == 'K')) {
                        size = size * 1024;
                    } else if ((s[0] == 'm') || (s[0] == 'M')) {
                        size = size * 1024 * 1024;
                    } else if ((s[0] == 'g') || (s[0] == 'G')) {
                        size = size * 1024 * 1024 * 1024;
                    } else {
                        n = -1;
                    }
                }
                if (n > 0) {
                    add_message(size);
                } else {
                    if (imb_p2p_config.rank == 0) {
                        fprintf(unit, "Unexpected format after \"-msgsz\"\n");
                        fflush(unit);
                    }
                    exit(1);
                }
            }
        } else if (!STRCASECMP(argv[i], "-msglog")) {
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-msglog\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                int min_log = 0;
                int max_log = 0;
                int n = sscanf(argv[i], "%d:%d", &min_log, &max_log);
                int ok = 0;
                if (n == 2) {
                    if ((min_log >= 0) && (max_log >= 0) && (max_log >= min_log) && (max_log <= IMB_P2P_MAX_MSG_LOG)) {
                        size_t size = ((size_t)1) << min_log;
                        size_t max_size = ((size_t)1) << max_log;
                        free_messages();
                        while (size <= max_size) {
                            add_message(size);
                            size *= 2;
                        }
                        ok = 1;
                    }
                } else if (n == 1) {
                    if ((min_log >= 0) && (min_log <= IMB_P2P_MAX_MSG_LOG)) {
                        size_t size = 1;
                        size_t max_size = ((size_t)1) << min_log;
                        free_messages();
                        add_message(0);
                        while (size <= max_size) {
                            add_message(size);
                            size *= 2;
                        }
                        ok = 1;
                    }
                }
                if (!ok) {
                    if (imb_p2p_config.rank == 0) {
                        fprintf(unit, "Unexpected -msglog argument value. Expected: <num1>:<num2>\n");
                        fprintf(unit, "where num1 and num2 are positive integer numbers, and num2>=num1, and num2<=%d\n", IMB_P2P_MAX_MSG_LOG);
                        fflush(unit);
                    }
                    exit(1);
                }
            }
        } else if (!STRCASECMP(argv[i], "-msglen")) {
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-msglen\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                const char * file_name = argv[i];
                FILE* file = fopen(file_name, "r");
                if (!file) {
                    if (imb_p2p_config.rank == 0) {
                        fprintf(unit, "Unable to open msglen file: %s\n", file_name);
                        fflush(unit);
                    }
                    exit(1);
                } else {
                    char line[72];
                    char s[sizeof(line)];
                    free_messages();
                    while (fgets(line, sizeof(line), file)) {
                        s[0] = '\0';
                        if ((line[0] != '#') && (strlen(line) > 0)) {
                            int d = 0;
                            int n = sscanf(&line[0], "%d%s", &d, &s[0]);
                            size_t size = d;
                            if ((n <= 0) || (n == EOF) || (d < 0)) {
                                n = -1;
                            } else if (n == 2) {
                                if ((s[0] == 'k') || (s[0] == 'K')) {
                                    size = size * 1024;
                                } else if ((s[0] == 'm') || (s[0] == 'M')) {
                                    size = size * 1024 * 1024;
                                } else if ((s[0] == 'g') || (s[0] == 'G')) {
                                    size = size * 1024 * 1024 * 1024;
                                } else {
                                    n = -1;
                                }
                            }
                            if (n > 0) {
                                add_message(size);
                            } else {
                                fclose(file);
                                if (imb_p2p_config.rank == 0) {
                                    fprintf(unit, "Unexpected format of the msglen file: %s\n", file_name);
                                    fflush(unit);
                                }
                                exit(1);
                            }
                        }
                    }
                    fclose(file);
                }
            }
        } else if (!STRCASECMP(argv[i], "-iter")) {
            i++;
            if (i >= argc) {
                if (imb_p2p_config.rank == 0) {
                    fprintf(unit, "Missing argument after \"-iter\"\n");
                    fflush(unit);
                }
                exit(1);
            } else {
                int d0 = 0;
                int d1 = 0;
                int n = sscanf(argv[i], "%d,%d", &d0, &d1);
                if (n == 1) {
                    if (d0 > 0) {
                        imb_p2p_config.iter.max = d0;
                    }
                } else if (n == 2) {
                    if ((d0 > 0) && (d1 > 0)) {
                        imb_p2p_config.iter.max = d0;
                        imb_p2p_config.iter.numerator = ((size_t)d1) * 1024 * 1024;
                    }
                }
            }
        }
    }
    if (imb_p2p_config.messages.array == NULL) {
        size_t size = 1;
        add_message(0);
        while (size <= IMB_P2P_DEFAULT_MAX_MESSAGE_SIZE) {
            add_message(size);
            size *= 2;
        }
    }
    if (!imb_p2p_config.benchmarks.array) {
        add_benchmark(IMB_P2P_PINGPONG, imb_p2p_pingpong);
        add_benchmark(IMB_P2P_PINGPING, imb_p2p_pingping);
        add_benchmark(IMB_P2P_UNIRANDOM, imb_p2p_unirandom);
        add_benchmark(IMB_P2P_BIRANDOM, imb_p2p_birandom);
        add_benchmark(IMB_P2P_CORANDOM, imb_p2p_corandom);
        add_benchmark(IMB_P2P_STENCIL2D, imb_p2p_stencil2d);
        add_benchmark(IMB_P2P_STENCIL3D, imb_p2p_stencil3d);
        add_benchmark(IMB_P2P_SENDRECV_REPLACE, imb_p2p_sendrecv_replace);
    }
    if (imb_p2p_config.rank == 0) {
        print_main_header(argc, argv);
    }
}

static void finalization() {
    imb_p2p_barrier(MPI_COMM_WORLD);
    if (imb_p2p_config.rank == 0) {
        print_main_footer();
    }
    free_benchmarks();
    free_messages();
    MPI_Finalize();
}

static void loading(int argc, char **argv) {
    int i;
    unit = stdout;
    imb_p2p_config.iter.numerator = 100 * (8 * 1024 * 1024);
    imb_p2p_config.iter.max = 100000;
    imb_p2p_config.payload.is_touch_send_buff = 1;
    imb_p2p_config.payload.is_touch_recv_buff = 1;
    imb_p2p_config.pause_usec = 100000;
    for (i = 0; i < argc; i++) {
        if (!STRCASECMP(argv[i], "-h") || !STRCASECMP(argv[i], "-help")) {
            print_title();
            i++;
            if (i < argc) {
                if (!STRCASECMP(argv[i], "list") || !STRCASECMP(argv[i], "-list")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-list]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Prints out all the benchmark names available in this IMB build.\n");
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "iter") || !STRCASECMP(argv[i], "-iter")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-iter max]\n");
                    fprintf(unit, "# Option: [-iter max,numerator]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Define number of iterations per sample by the rule:\n");
                    fprintf(unit, "# number_of_iterations = (numerator * 1024 * 1024) / size\n");
                    fprintf(unit, "# number_of_iterations <= max\n");
                    fprintf(unit, "# number_of_iterations >= 1\n");
                    fprintf(unit, "# Default -iter %" PRIu64 ",%" PRIu64 "\n", imb_p2p_config.iter.max, (imb_p2p_config.iter.numerator / (1024 * 1024)));
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "msglen") || !STRCASECMP(argv[i], "-msglen")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-msglen filename]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# The argument after -msglen is a file name.\n");
                    fprintf(unit, "# The file should be in ASCII encoding and contain set of message length, 1 per line\n");
                    fprintf(unit, "# The following message length suffixes are allowed: K, M, G.\n");
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "msglog") || !STRCASECMP(argv[i], "-msglog")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-msglog n]\n");
                    fprintf(unit, "# Option: [-msglog k:n]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Defines set of message length:\n");
                    fprintf(unit, "# {0, 1, 2, 4, ..., 2^(n-1), 2^n}\n");
                    fprintf(unit, "# {2^k, 2^(k+1), ..., 2^(n-1), 2^n}\n");
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "msgsz") || !STRCASECMP(argv[i], "-msgsz")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-msgsz size]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# The argument after -msgsz is a size of a message in bytes.\n");
                    fprintf(unit, "# The following suffixes are allowed: K, M, G.\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Example: -msgsz 4097 -msgsize 5k -msgsize 2m \n");
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "msgwr") || !STRCASECMP(argv[i], "-msgwr")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-msgwr false | true | 0 | 1 | off | on | disable | enable | no | yes]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Enable or disable touch (write) the egress buffer cache lines before send a message\n");
                    fprintf(unit, "# Default -msgwr %s\n", (imb_p2p_config.payload.is_touch_send_buff ? "true" : "false"));
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "msgrd") || !STRCASECMP(argv[i], "-msgrd")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-msgrd false | true | 0 | 1 | off | on | disable | enable | no | yes]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Enable or disable touch (read) the ingress buffer cache lines after receive a message\n");
                    fprintf(unit, "# Default -msgrd %s\n", (imb_p2p_config.payload.is_touch_recv_buff ? "true" : "false"));
                    fprintf(unit, "#\n");
                    exit(0);
                } else if (!STRCASECMP(argv[i], "pause") || !STRCASECMP(argv[i], "-pause")) {
                    fprintf(unit, "#\n");
                    fprintf(unit, "# Option: [-pause microseconds]\n");
                    fprintf(unit, "#\n");
                    fprintf(unit, "# The argument after -pause is a microseconds time interval between runs.\n");
                    fprintf(unit, "# Default -pause %d\n", imb_p2p_config.pause_usec);
                    fprintf(unit, "#\n");
                    exit(0);
                }
            }
            fprintf(unit, "#\n");
            fprintf(unit, "# Usage: IMB-P2P [-help]\n");
            fprintf(unit, "#                [-help option]\n");
            fprintf(unit, "#                [-list]\n");
            fprintf(unit, "#                [-iter max]\n");
            fprintf(unit, "#                [-iter max,numerator]\n");
            fprintf(unit, "#                [-msglen filename]\n");
            fprintf(unit, "#                [-msglog n]\n");
            fprintf(unit, "#                [-msglog k:n]\n");
            fprintf(unit, "#                [-msgsz size]\n");
            fprintf(unit, "#                [-msgrd false | true | 0 | 1 | off | on | disable | enable | no | yes]\n");
            fprintf(unit, "#                [-msgwr false | true | 0 | 1 | off | on | disable | enable | no | yes]\n");
            fprintf(unit, "#                [-pause microseconds]\n");
            fprintf(unit, "#                [benchmark[,benchmark,[...]]]\n");
            fprintf(unit, "#\n");
            exit(0);
        }
        if (!STRCASECMP(argv[i], "-l") || !STRCASECMP(argv[i], "-list")) {
            print_title();
            fprintf(unit, "# List of Benchmarks to run:\n");
            fprintf(unit, "#\n");
            fprintf(unit, "# %s\n", IMB_P2P_PINGPONG);
            fprintf(unit, "# %s\n", IMB_P2P_PINGPING);
            fprintf(unit, "# %s\n", IMB_P2P_UNIRANDOM);
            fprintf(unit, "# %s\n", IMB_P2P_BIRANDOM);
            fprintf(unit, "# %s\n", IMB_P2P_CORANDOM);
            fprintf(unit, "# %s\n", IMB_P2P_STENCIL2D);
            fprintf(unit, "# %s\n", IMB_P2P_STENCIL3D);
            fprintf(unit, "# %s\n", IMB_P2P_SENDRECV_REPLACE);
            fprintf(unit, "#\n");
            fflush(unit);
            exit(0);
        }
    }
}

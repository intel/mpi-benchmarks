--------------------------------------
Intel(R) MPI Benchmarks 2021.8
README
--------------------------------------

--------
Contents
--------

- Introduction
- Product Directories
- What's New
- Command-Line Control
- Building Instructions for Linux* OS
- Building Instructions for Windows* OS
- Copyright and License Information
- Legal Information

------------
Introduction
------------
Intel(R) MPI Benchmarks provides a set of elementary benchmarks that conform
to MPI-1, MPI-2, and MPI-3 standard.
You can run all of the supported benchmarks, or a subset specified in the
command line using one executable file. Use command-line parameters to specify
various settings, such as time measurement, message lengths, and selection of 
communicators. For details, see the Intel(R) MPI Benchmarks User's Guide 
located in the <install-dir>/doc directory.

By default, Intel(R) MPI Benchmarks is installed at:
  - C:\Program Files (x86)\IntelSWTools\imb on Windows* OS 
  - /opt/intel/imb on Linux* OS

Before using the Intel(R) MPI Benchmarks, please read the license agreements 
located in the imb/license directory.

-------------------
Product Directories
-------------------
After a successful installation of Intel(R) MPI Benchmarks, the following 
files and folders appear on your system:

    +-- \imb            Intel(R) MPI Benchmarks product directory
         |
         +-- \src_c             Product source "C" code and Makefiles.
         |
         +-- \license           Product license files.
         |    |              
         |    +--license.txt    Source code license granted to you.
         |    |                             
         |    +--use-of-trademark-license.txt    License file describing the 
         |                                       use of the Intel(R) MPI 
         |                                       Benchmarks name and trademark.
         |
         +-- \src_cpp              Product source "CPP" code and Makefiles. 
         |
         +-- \WINDOWS              Microsoft* Visual Studio* project files. 
         |
         +-- Readme_IMB.txt        Readme file providing the basic information
                                   about the product (this file).

----------
What's New
----------
New in Intel(R) MPI Benchmarks 2021.8
----------------------------------------
- Migration to icx,icpx compiler
- IMB-MPI1: FP16/BF16 data_type support
- Bug fixes

New in Intel(R) MPI Benchmarks 2021.7
----------------------------------------
- IMB-MPI1-GPU benchmarks support cuda.
  Dynamically loading from LD_LIBRARY_PATH
  cuda or level zero library.

New in Intel(R) MPI Benchmarks 2021.6
----------------------------------------
- Bug fixes.

New in Intel(R) MPI Benchmarks 2021.5
----------------------------------------
- License update
- Bug fixes.

New in Intel(R) MPI Benchmarks 2021.4
----------------------------------------
- Bug fixes.

New in Intel(R) MPI Benchmarks 2021.3
----------------------------------------
- Change default value for mem_alloc_type to device
- License update
- Bug fixes.

New in Intel(R) MPI Benchmarks 2021.2
----------------------------------------
- New IMB-MPI1-GPU benchmarks (Technical Preview).
  The benchmarks implement the GPU version of the IMB-MPI1
- Added -msg_pause option.
- Changed default window_size 64 -> 256
- Bug fixes.


New in Intel(R) MPI Benchmarks 2021.1
----------------------------------------
- Added -window_size option for IMB-MPI1
- Bug fixes.


New in Intel(R) MPI Benchmarks 2021.1-beta05
----------------------------------------
- Added copyrights for *.exe
- Bug fixes.


New in Intel(R) MPI Benchmarks 2021.1-beta02
----------------------------------------
- New IMB-P2P Stencil2D and Stencil3D benchmarks.
- Bug fixes.


New in Intel(R) MPI Benchmarks 2021.1-beta01
----------------------------------------
- Added Visual Studio projects for IMB-P2P
- Bug fixes.


New in Intel(R) MPI Benchmarks 2019 Update 3
----------------------------------------
- Added the warm_up option that enabled additional cycles before running benchmark(for all size.)
- Added a new benchmark Reduce_local for IMB-MPI1.
- Bug fixes.


New in Intel(R) MPI Benchmarks 2019 Update 2
----------------------------------------
- New IMB-P2P benchmarks.
- Added the Reduce_local benchmark for IMB-MPI1.
- Deleted the alignment option (-alignment).
- Bug fixes.
- Code cleanup.


New in Intel(R) MPI Benchmarks 2019 Update 1
----------------------------------------
- Added the Reduce_scatter_block benchmark for IMB-MPI1.
- Added the aggregate_mode option that specifies the mode for IMB-IO, IMB-EXT and IMB-RMA.
- Added the alignment option that controls buffer alignment.
- Updated the following options:
    - -data_type now supports double.
    - -red_data_type now supports double.


New in Intel(R) MPI Benchmarks 2019
----------------------------------------
- New IMB-MT benchmarks.
  The benchmarks implement the multi-threaded version of IMB-MPI1
  benchmarks using the OpenMP* paradigm.
- New benchmarks infrastructure for easier benchmarks extension is implemented in C++
  (See the guide:  https://www.intel.com/content/www/us/en/developer/articles/technical/creating-custom-benchmarks-for-imb-2019.html?wapkw=creating-custom-benchmarks-for-imb-2019).
  The IMB-MPI1, IMB-RMA, IMB-NBC, IMB-EXT, IMB-IO and IMB-MT implementation is now based on the new C++
  infrastructure.
  The legacy infrastructure is preserved in the src_c subdirectory.
- Syntax changes for the -include and -exclude options.
  Benchmarks to include and exclude now must be separated by a comma rather
  than a space. Benchmarks to launch can be separated by a comma or a space.
- Iteration policy can no longer be set with the -iter option. Use the -iter_policy
  instead.
- Added a new benchmark BarrierMT for IMB-MT.
- Added new options:
    - -noheader for IMB-MT disables printing of benchmark headers.
    - -data_type for IMB-MPI1 specifies the type to be used for communication.
    - -red_data_type for IMB-MPI1 specifies the type to be used for reduction.
    - -contig_type for IMB-MPI1 specifies the type to be used.
    - -zero_size for IMB-MPI1 disable runs with message size 0.
- Bug fixes.
- Code cleanup.

New in Intel(R) MPI Benchmarks 2018 Update 1
--------------------------------------------
- Support for the Microsoft* Visual Studio* 2017. Microsoft* Visual Studio* 2012
  support is removed.

New in Intel(R) MPI Benchmarks 2018
--------------------------------------------
- Product documentation is now available online only at:
  https://www.intel.com/content/www/us/en/docs/mpi-library/user-guide-benchmarks/2021-2/overview.html
- Removed support of the Intel(R) Xeon Phi(TM) coprocessors (formerly code named
  Knights Corner).

New in Intel(R) MPI Benchmarks 2017 Update 1
--------------------------------------------
- Added a new option -imb_barrier.
- The PingPong and PingPing benchmarks are now equivalent to PingPongSpecificSource
  and PingPingSpecificSource, respectively. Their old behavior (with MPI_ANY_SOURCE)
  is available in PingPongAnySource and PingPingAnySource.

New in Intel(R) MPI Benchmarks 2017
-------------------------------------------
- Changed default values for the -sync and -root_shift options.
- Support for the Microsoft* Visual Studio* 2015. Microsoft* Visual Studio* 2010
support is removed.
- Bug fixes.

New in Intel(R) MPI Benchmarks 4.1 Update 1
-------------------------------------------
- Bug fixes.

New in Intel(R) MPI Benchmarks 4.1
-------------------------------------------
- Introduced two new benchmarks: uniband and biband.
- Introduced two new command-line options for collective benchmarks: -sync and -root_shift.

New in Intel(R) MPI Benchmarks 4.0 Update 2
-------------------------------------------
- Fix of a bug where benchmarking was failing on certain message lengths with -DCHECK.

New in Intel(R) MPI Benchmarks 4.0 Update 1
-------------------------------------------
- Fix of a bug where benchmarking could continue after the time limit is exceeded.

New in Intel(R) MPI Benchmarks 4.0
-------------------------------------------
- Introduced new components IMB-NBC and IMB-RMA that conform to the MPI-3.0 
standard.
  Note: These components can only be built and used with MPI libraries that conform 
  to the MPI-3 standard.
- Added new targets to the Linux* OS Makefiles:
    - NBC for building IMB-NBC
    - RMA for building IMB-RMA 
- Updated Microsoft* Visual Studio* solutions to include the IMB-NBC and 
IMB-RMA targets.
- Consolidated all first-use documents in ReadMe_IMB.txt to improve usability.
- Introduced a new feature to set the appropriate algorithm for automatic calculation
of iterations. The algorithm can be set through the -iter and -iter_policy options.
- Support for the Microsoft* Visual Studio* 2013. Microsoft* Visual Studio* 2008
support is removed.

--------------------
Command-Line Control
--------------------

You can get help on the Intel(R) MPI Benchmarks from the command line using 
the component name and the -help parameter. For example, for the IMB-MPI1 
component, run:
IMB-MPI1 -help

You can see the Intel(R) MPI Benchmarks User's Guide for details on the 
command-line parameters.

-----------------------------------------
Building Instructions for Linux* OS
-----------------------------------------
1) Set the CC and CXX variables to point to the appropriate compiler wrapper,
   mpiicx or mpicc for C, mpiicpx or mpicxx for C++.
2) Run one or more Makefile commands below:

   make clean - remove legacy binary object files and executable files
   make IMB-MPI1 - build the executable file for the IMB-MPI1 component
   make IMB-EXT - build the executable file for one-sided communications benchmarks
   make IMB-IO - build the executable file for I/O benchmarks
   make IMB-NBC - build the executable file for IMB-NBC benchmarks
   make IMB-RMA - build the executable file for IMB-RMA benchmarks
   make all - build all executable files available
   
3) Run the benchmarks as follows:

   mpirun -n <number_of_processes> IMB-<component> [arguments]

   where <component> is one of the make targets above.
   For details, refer to the Intel(R) MPI Benchmarks User's Guide at:
   https://www.intel.com/content/www/us/en/docs/mpi-library/user-guide-benchmarks/2021-2/overview.html

-----------------------------------------
Building Instructions for Windows* OS
-----------------------------------------
Use the enclosed solution files located in the component-specific 
subdirectories under the imb/WINDOWS directory. Click on the respective 
".vcproj" or ".vcxproj" project file and use the Microsoft* Visual Studio* 
menu to run the associated benchmark application.

Building "x64" Executable Files 
-------------------------------
1) Check that the Include, Lib, and Path environment variables are set as follows:
    %I_MPI_ROOT%\intel64\include
    %I_MPI_ROOT%\intel64\lib
    %I_MPI_ROOT%\mpi\intel64\bin
   The %I_MPI_ROOT% environment variable is set to the Intel(R) MPI Library 
   installation directory.

2) Open the ".vcproj" or ".vcxproj" file for the component you would like to 
   build. From the Visual Studio Project panel:
   a) Change the "Solution Platforms" dialog box to "x64".
   b) Change the "Solution Configurations" dialog box to "Release".
   c) Check other settings as required, for example:
    General > Project Defaults
       - Set "Character Set" to "Use Multi-Byte Character Set"
    C/C++ > General 
       - Set "Additional Include Directories" to 
           "$(I_MPI_ROOT)\intel64\include"
       - Set "Warning Level" to "Level 1 (/W1)"
    C/C++ > Preprocessor
       - For the "Preprocessor definitions" within the Visual Studio 
         projects, add the conditional compilation macros WIN_IMB and 
         _CRT_SECURE_NO_DEPRECATE. Depending on the components you intend to 
         use, add one or more of the following macros: 
         MPI1, EXT, MPIIO, NBC, RMA.
    Linker > Input
       - Set "Additional Dependencies" to "$(I_MPI_ROOT)\intel64\lib\impi.lib". 
         Make sure to add quotes.

3) Use F7 or Build > Build Solution to create an executable.

   For details, refer to the Intel(R) MPI Benchmarks User's Guide at:
   https://www.intel.com/content/www/us/en/docs/mpi-library/user-guide-benchmarks/2021-2/overview.html

----------------------
Copyright and Licenses
----------------------

See the license files in the imb/license directory.

--------------------------------
Legal Information
--------------------------------
No license (express or implied, by estoppel or otherwise) to any intellectual
property rights is granted by this document.

Intel disclaims all express and implied warranties, including without limitation,
the implied warranties of merchantability, fitness for a particular purpose, and
non-infringement, as well as any warranty arising from course of performance,
course of dealing, or usage in trade.

This document contains information on products, services and/or processes in
development. All information provided here is subject to change without notice.
Contact your Intel representative to obtain the latest forecast, schedule,
specifications and roadmaps.

The products and services described may contain defects or errors known as
errata which may cause deviations from published specifications. Current
characterized errata are available on request.

Intel, Intel Core, Xeon, Xeon Phi and the Intel logo are trademarks of Intel
Corporation in the U.S. and/or other countries.

* Other names and brands may be claimed as the property of others.

(C) Intel Corporation.

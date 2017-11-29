--------------------------------------------------
Intel(R) MPI Benchmarks 2018 Update 1
README
--------------------------------------------------

--------
Contents
--------

- Introduction
- Product Directories
- What's New
- Command-Line Control
- Build Instructions for Linux* OS
- Build Instructions for Windows* OS
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
located at: https://software.intel.com/en-us/imb-user-guide

When installed as part of Intel(R) MPI Library, the default location of
the Intel(R) MPI Benchmarks is:
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
         +-- \license           Product license files.
         |    |              
         |    +--license.txt    Source code license granted to you.
         |    |                             
         |    +--use-of-trademark-license.txt    License file describing the 
         |                                       use of the Intel(R) MPI 
         |                                       Benchmarks name and trademark.
         |
         +-- \src                  Product source code and Makefiles. 
         |
         +-- \WINDOWS              Microsoft* Visual Studio* project files. 
         |
         +-- Readme_IMB.txt        Readme file providing the basic information
                                   about the product (this file).

----------
What's New
----------
New in Intel(R) MPI Benchmarks 2018 Update 1
--------------------------------------------
- Support for the Microsoft* Visual Studio* 2017. Microsoft* Visual Studio* 2012 
support is removed.

New in Intel(R) MPI Benchmarks 2018
--------------------------------------------
- Product documentation is now available online only at:
  https://software.intel.com/en-us/imb-user-guide
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
Build Instructions for Linux* OS
-----------------------------------------

1) Set up the environment for the compiler and Intel(R) MPI Library.
   For the Intel(R) compilers, run:
   
   source <compiler_dir>/bin/compilervars.sh intel64
   
   For the Intel(R) MPI Library, run:
   
   source <intel_mpi_dir>/intel64/bin/mpivars.sh

2) Set the CC variable to point to the appropriate compiler wrapper, mpiicc or mpicc.
3) Run one or more Makefile commands below:

   make clean - remove legacy binary object files and executable files
   make MPI1 - build the executable file for the IMB-MPI1 component
   make EXT - build the executable file for one-sided communications benchmarks
   make IO - build the executable file for I/O benchmarks
   make NBC - build the executable file for IMB-NBC benchmarks
   make RMA - build the executable file for IMB-RMA benchmarks
   make all - build all executable files available
   
4) Run the benchmarks as follows:

   mpirun -n <number_of_processes> IMB-<component> [arguments]

   where <component> is one of the make targets above.
   For details, refer to the Intel(R) MPI Benchmarks User's Guide.

-----------------------------------------
Build Instructions for Windows* OS
-----------------------------------------
Use the enclosed solution files located in the component-specific 
subdirectories under the imb/WINDOWS directory. Click on the respective 
".vcproj" or ".vcxproj" project file and use the Microsoft* Visual Studio* 
menu to run the associated benchmark application.

Use F7 or Build > Build Solution to create an executable.

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

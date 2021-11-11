# Intel(R) MPI Benchmarks <!-- omit in toc -->
[![Common Public License Version 1.0](https://img.shields.io/badge/license-Common%20Public%20License%20Version%201.0-green.svg)](license/license.txt)
![v2021.3](https://img.shields.io/badge/v.2021.3-Update-orange.svg)
--------------------------------------------------



## Table of Contents <!-- omit in toc -->
- [Introduction](#introduction)
- [Release Notes](#release-notes)
- [Documentation](#documentation)
- [Product Directories](#product-directories)
- [Command-Line Control](#command-line-control)
- [Building Instructions](#building-instructions)
  - [For Linux* OS](#for-linux*-os)
  - [For Windows* OS](#for-windows*-os)
- [Copyright and License](#copyright-and-license)
- [Legal Information](#legal-information)



## Introduction

Intel(R) MPI Benchmarks provides a set of elementary benchmarks that conform
to MPI-1, MPI-2, and MPI-3 standard.
You can run all of the supported benchmarks, or a subset specified in the
command line using one executable file. Use command-line parameters to specify
various settings, such as time measurement, message lengths, and selection of 
communicators.


By default, Intel(R) MPI Benchmarks is installed at:
  - on Windows* OS: `C:\Program Files (x86)\IntelSWTools\imb on Windows* OS`
  - on Linux* OS: `/opt/intel/imb` 



## Release Notes 

See [Release Notes](RELEASENOTES.md) to learn about new features and fixed issues.



## Documentation 
Documentation is available online. See [Intel(R) MPI Benchmarks User Guide](https://software.intel.com/en-us/imb-user-guide).



## Product Directories

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



## Command-Line Control

Get help on the Intel(R) MPI Benchmarks from the command line using 
the component name and the `-help` parameter. For example, for the IMB-MPI1 
component, run:
```IMB-MPI1 -help```



## Building Instructions

You can build Intel(R) MPI Benchmarks on Linux* OS and Windows* OS. 

### For Linux* OS

1) Set the CC variable to point to the appropriate compiler wrapper, mpiicc or 
   mpicc.
2) Run one or more following Makefile commands:

   - make clean - remove legacy binary object files and executable files
   - make IMB-MPI1 - build the executable file for the IMB-MPI1 component
   - make IMB-EXT - build the executable file for one-sided communications benchmarks
   - make IMB-IO - build the executable file for I/O benchmarks
   - make IMB-NBC - build the executable file for IMB-NBC benchmarks
   - make IMB-RMA - build the executable file for IMB-RMA benchmarks
   - make all - build all executable files available

3) Run the benchmarks as follows:

   ```mpirun -n <number_of_processes> IMB-<component> [arguments]```

   Where <component> is one of the make targets above.


### For Windows* OS

Use the enclosed solution files located in the component-specific 
subdirectories under the `imb/WINDOWS` directory. Click on the respective 
`.vcproj` or `.vcxproj` project file and use the Microsoft* Visual Studio* 
menu to run the associated benchmark application.

#### Building "x64" Executable Files 

1) Check that the Include, Lib, and Path environment variables are set as follows:
    - ```%I_MPI_ROOT%\intel64\include```
    - ```%I_MPI_ROOT%\intel64\lib```
    - ```%I_MPI_ROOT%\mpi\intel64\bin```
   
  The ``%I_MPI_ROOT%`` environment variable is set to the Intel(R) MPI Library 
   installation directory.

2) Open the `.vcproj` or `.vcxproj` file for the component you build. From the Visual Studio Project panel:
  
    a) Change the `Solution Platforms` dialog box to `x64`.
    b) Change the `Solution Configurations` dialog box to `Release`.
    c) Check other settings as required, for example:
    General > Project Defaults
       - Set `Character Set` to `Use Multi-Byte Character Set`
    C/C++ > General 
       - Set `Additional Include Directories` to 
           ```$(I_MPI_ROOT)\intel64\include```
       - Set `Warning Level` to `Level 1 (/W1)`
    C/C++ > Preprocessor
       - For the `Preprocessor definitions` within the Visual Studio 
         projects, add the conditional compilation macros `WIN_IMB` and 
         `_CRT_SECURE_NO_DEPRECATE`. Depending on the components you intend to 
         use, add one or more of the following macros: 
         MPI1, EXT, MPIIO, NBC, RMA.
    Linker > Input
       - Set `Additional Dependencies` to ```$(I_MPI_ROOT)\intel64\lib\impi.lib```. 
         Make sure to add quotes.

3) Use F7 or Build > Build Solution to create an executable.



## Copyright and Licenses

See the [license](license) for more information.



## Legal Information

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

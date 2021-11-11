# Release Notes


## Intel(R) MPI Benchmarks 2021.3

- Change default value for mem_alloc_type to device.
- License update.
- Bug fixes.


## Intel(R) MPI Benchmarks 2021.2

- New IMB-MPI1-GPU benchmarks (Technical Preview).
  The benchmarks implement the GPU version of the IMB-MPI1.
- Added `-msg_pause option`.
- Changed default window_size 64 to 256.
- Bug fixes.


## Intel(R) MPI Benchmarks 2021.1

- Added `-window_size` option for IMB-MPI1.
- Added copyrights for *.exe .
- Bug fixes.


## Intel(R) MPI Benchmarks 2019 Update 6

- New IMB-P2P Stencil2D and Stencil3D benchmarks.
- Bug fixes.


## Intel(R) MPI Benchmarks 2019 Update 5

- Added Visual Studio* projects for IMB-P2P.
- Bug fixes.


## Intel(R) MPI Benchmarks 2019 Update 4

- Bug fixes.


## Intel(R) MPI Benchmarks 2019 Update 3

- Added the warm_up option that enabled additional cycles before running benchmark for all sizes.
- Added a new benchmark `Reduce_local` for IMB-MPI1.
- Bug fixes.


## New in Intel(R) MPI Benchmarks 2019 Update 2

- New IMB-P2P benchmarks.
- Added the `Reduce_local` benchmark for IMB-MPI1.
- Deleted the `-alignment` option.
- Bug fixes.
- Code cleanup.


## Intel(R) MPI Benchmarks 2019 Update 1

- Added the `Reduce_scatter_bloc`k benchmark for IMB-MPI1.
- Added the `aggregate_mode` option that specifies the mode for IMB-IO, IMB-EXT, and IMB-RMA.
- Added the `-alignment` option that controls buffer alignment.
- Updated the following options that supports double now:
    - `-data_type` 
    - `-red_data_type` 


## Intel(R) MPI Benchmarks 2019

- New IMB-MT benchmarks.
  The benchmarks implement the multi-threaded version of IMB-MPI1 benchmarks 
  using the OpenMP* paradigm.
- New benchmarks infrastructure for easier benchmarks extension is 
  implemented in C++. See the [guide](https://software.intel.com/en-us/articles/creating-custom-benchmarks-for-imb-2019).
  The IMB-MPI1, IMB-RMA, IMB-NBC, IMB-EXT, IMB-IO, and IMB-MT implementations 
  are based on the new C++ infrastructure now. The legacy infrastructure is 
  preserved in the `src_c` subdirectory.
- Syntax changes for the `-include` and `-exclude` options.
  Benchmarks to include and exclude must be separated by a comma rather
  than a space. Benchmarks to launch can be separated by a comma or a space.
- Iteration policy can no longer be set with the `-iter` option. Use the 
  `-iter_policy` instead.
- Added a new benchmark BarrierMT for IMB-MT.
- Added new options:
    - `-noheader` for IMB-MT disables printing of benchmark headers.
    - `-data_type` for IMB-MPI1 specifies the type used for communication.
    - `-red_data_type` for IMB-MPI1 specifies the type used for reduction.
    - `-contig_type` for IMB-MPI1 specifies the used type.
    - `-zero_siz`e for IMB-MPI1 disable runs with message size 0.
- Bug fixes.
- Code cleanup.


## Intel(R) MPI Benchmarks 2018 Update 1

- Support for the Microsoft* Visual Studio* 2017. Microsoft* Visual Studio* 2012
  support is removed.

## Intel(R) MPI Benchmarks 2018

- Product documentation is now available online only at:
  https://software.intel.com/en-us/imb-user-guide
- Removed support of the Intel(R) Xeon Phi(TM) coprocessors. Formerly code named
  Knights Corner).


## Intel(R) MPI Benchmarks 2017 Update 1

- Added a new option `-imb_barrier`.
- The PingPong and PingPing benchmarks are now equivalent to 
  PingPongSpecificSource and PingPingSpecificSource, respectively. Their old 
  behavior (with `MPI_ANY_SOURCE`) is available in PingPongAnySource and 
  PingPingAnySource.


## Intel(R) MPI Benchmarks 2017

- Changed default values for the `-sync` and `-root_shift` options.
- Support for the Microsoft* Visual Studio* 2015. Microsoft* Visual Studio* 2010
  support is removed.
- Bug fixes.


## Intel(R) MPI Benchmarks 4.1 Update 1

- Bug fixes.


## Intel(R) MPI Benchmarks 4.1

- Introduced two new benchmarks: uniband and biband.
- Introduced two new command-line options for collective benchmarks: `-sync` and 
  `-root_shift`.


## Intel(R) MPI Benchmarks 4.0 Update 2

- Fixed a bug where benchmarking was failing on certain message lengths with 
  `-DCHECK`.


## New in Intel(R) MPI Benchmarks 4.0 Update 1

- Fixed a bug where benchmarking could continue after the time limit is exceeded.


## Intel(R) MPI Benchmarks 4.0

- Introduced new components IMB-NBC and IMB-RMA that conform to the MPI-3.0 
  standard.
  **Note**: These components can only be built and used with MPI libraries that conform to the MPI-3 standard.
- Added new targets to the Linux* OS Makefiles:
    - NBC for building IMB-NBC
    - RMA for building IMB-RMA 
- Updated Microsoft* Visual Studio* solutions to include the IMB-NBC and IMB-RMA 
  targets.
- Consolidated all first-use documents in ReadMe_IMB.txt to improve usability.
- Introduced a new feature to set the appropriate algorithm for automatic 
  calculation of iterations. The algorithm can be set through the `-iter` and 
  `-iter_policy` options.
- Support for the Microsoft* Visual Studio* 2013. Microsoft* Visual Studio* 2008
  support is removed.
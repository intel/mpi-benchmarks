default: MPI1

all: EXT IO MPI1 NBC RMA

MPI1: feedback
	gmake -f make_ict IMB-MPI1

EXT:
	gmake -f make_ict IMB-EXT

IO:
	gmake -f make_ict IMB-IO

NBC:
	gmake -f make_ict IMB-NBC

RMA:
	gmake -f make_ict IMB-RMA

feedback:
	@echo " "
	@echo "This Makefile uses make_ict to build IMB "
	@echo "based on Intel(R) Cluster Studio"
	@echo " "
	@echo "Targets: MPI1 (default), EXT, IO, NBC, all, clean"
	@echo "         MPI1 builds the MPI-1 standard benchmarks"
	@echo "         EXT builds the one sided communications benchmarks"
	@echo "         IO builds the MPI-File I\/O benchmarks"
	@echo "         NBC builds the nonblocking collective operations benchmarks"
	@echo "         RMA builds the one-sided benchmarks (MPI-3 plus passive mode)"
	@echo "         all = MPI1+EXT+IO+NBC"
	@echo " "
	@echo "When an Intel(R) MPI Library install and mpiicc path exists,"
	@echo "this should work immediately."
	@echo " "
	@echo "Alternatively, use "
	@echo " "
	@echo "gmake -f make_mpich"
	@echo " "
	@echo "to install an mpich or similar version; for this,"
	@echo "you normally have to edit at least the MPI_HOME"
	@echo "variable provided in make_mpich"
	@echo " "

clean:
	/bin/rm -f *.o *~ PI* core IMB-IO IMB-EXT IMB-MPI1 IMB-NBC IMB-RMA exe_io exe_ext exe_mpi1 exe_nbc exe_rma IMB_out IMB_out_*

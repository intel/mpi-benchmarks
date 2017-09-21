all: IMB-MPI1 IMB-NBC IMB-RMA IMB-EXT IMB-IO IMB-MT

IMB-MPI1:
	make -j8 -C src -f Makefile TARGET=MPI1
	@cp src/IMB-MPI1 .

IMB-NBC:
	make -C legacy -f Makefile TARGET=NBC
	@cp legacy/IMB-NBC .

IMB-EXT:
	make -C legacy -f Makefile TARGET=EXT
	@cp legacy/IMB-EXT .

IMB-RMA:
	make -C legacy -f Makefile TARGET=RMA
	@cp legacy/IMB-RMA .

IMB-IO:
	make -C legacy -f Makefile TARGET=IO
	@cp legacy/IMB-IO .

IMB-MT: | IMB-MPI1
	make -j8 -C src -f Makefile TARGET=MT
	@cp src/IMB-MT .


clean:
	make -C src -f Makefile TARGET=MPI1 clean
	make -C legacy -f Makefile TARGET=NBC clean
	make -C legacy -f Makefile TARGET=RMA clean
	make -C legacy -f Makefile TARGET=EXT clean
	make -C legacy -f Makefile TARGET=IO clean
	make -C src -f Makefile TARGET=MT clean
	rm -f IMB-MPI1 IMB-NBC IMB-RMA IMB-EXT IMB-IO IMB-MT

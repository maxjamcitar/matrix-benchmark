OPTFLAG				= O3
OPENBLASFLAGS		= 
LOCOPENBLASSRC		= Dependencies/OpenBLAS
LOCOPENBLASRUN		= Dependencies/OpenBLAS-run
MATRIXOBJFILE		= matrix

all: $(MATRIXOBJFILE)
	echo "Compilation of matrix-benchmark and OpenBLAS started."

$(LOCOPENBLASRUN): 
	(cd $(LOCOPENBLASSRC) && make $(OPENBLASFLAGS) ) 
	mkdir -p $(LOCOPENBLASRUN)
	(cd $(LOCOPENBLASSRC) && make PREFIX="../../$(LOCOPENBLASRUN)" install)

$(MATRIXOBJFILE): $(LOCOPENBLASRUN)
	gcc matrix.c -o $(MATRIXOBJFILE) -I $(LOCOPENBLASRUN)/include -L$(LOCOPENBLASRUN)/lib -lopenblas -lm -$(OPTFLAG)

clean: 
	rm -rf *.o 	$(MATRIXOBJFILE)
	rm -rf 		$(LOCOPENBLASRUN)
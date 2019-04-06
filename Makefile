GCCFLAGS			= -O3
OPENBLASFLAGS		= 
LOCOPENBLASSRC		= Dependencies/OpenBLAS
LOCOPENBLASRUN		= Dependencies/OpenBLAS-run
MATRIXOBJFILE		= matrix-obj

.PHONY: all clean

all: $(MATRIXOBJFILE)
	
$(LOCOPENBLASRUN): 
	(cd $(LOCOPENBLASSRC) && make $(OPENBLASFLAGS) ) 
	mkdir -p $(LOCOPENBLASRUN)
	(cd $(LOCOPENBLASSRC) && make PREFIX="../../$(LOCOPENBLASRUN)" install)

$(MATRIXOBJFILE): matrix.c $(LOCOPENBLASRUN)
	gcc matrix.c -o $(MATRIXOBJFILE) -I$(LOCOPENBLASRUN)/include -Wl,-rpath,$(LOCOPENBLASRUN)/lib -L$(LOCOPENBLASRUN)/lib  -lopenblas -lm -Wall $(GCCFLAGS)

clean: 
	rm -rf *.o 	$(MATRIXOBJFILE) 
	rm -rf 		$(LOCOPENBLASRUN)

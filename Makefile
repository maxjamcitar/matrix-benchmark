OPTFLAG				= O3
OPENBLASFLAGS		= 
LOCOPENBLASSRC		= Dependencies/OpenBLAS
LOCOPENBLASRUN		= Dependencies/OpenBLAS-run
MATRIXDGEMMOBJFILE	= matrix-dgemm
MATRIXDEFOBJFILE	= matrix-def

.PHONY: all clean

all: $(MATRIXDGEMMOBJFILE) $(MATRIXDEFOBJFILE)
	

$(MATRIXDEFOBJFILE): matrix-def.c
	gcc matrix-def.c -o $(MATRIXDEFOBJFILE) -lm -Wall -$(OPTFLAG)

$(LOCOPENBLASRUN): 
	(cd $(LOCOPENBLASSRC) && make $(OPENBLASFLAGS) ) 
	mkdir -p $(LOCOPENBLASRUN)
	(cd $(LOCOPENBLASSRC) && make PREFIX="../../$(LOCOPENBLASRUN)" install)

$(MATRIXDGEMMOBJFILE): matrix-dgemm.c $(LOCOPENBLASRUN)
	gcc matrix-dgemm.c -o $(MATRIXDGEMMOBJFILE) -I$(LOCOPENBLASRUN)/include -Wl,-rpath,$(LOCOPENBLASRUN) -L$(LOCOPENBLASRUN)/lib  -lopenblas -lm -Wall -$(OPTFLAG)

clean: 
	rm -rf *.o 	$(MATRIXDGEMMOBJFILE) $(MATRIXDEFOBJFILE)
	rm -rf 		$(LOCOPENBLASRUN)
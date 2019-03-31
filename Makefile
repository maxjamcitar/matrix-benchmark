OPTFLAG=O3

all: matrix
	

matrix:
	gcc matrix.c -o matrix -I Dependencies/OpenBLAS-run/include -LDependencies/OpenBLAS-run/lib -lopenblas -lm -$(OPTFLAG)

clean: 
	rm -rf *.o matrix
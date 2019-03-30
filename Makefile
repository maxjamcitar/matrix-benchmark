OPTFLAG=O3

all: matrix

matrix:
	gcc matrix.c -o matrix -I Dependencies/OpenBLAS/include -LDependencies/OpenBLAS/lib -lopenblas -lm -$(OPTFLAG)

clean: 
	rm -rf *.o matrix
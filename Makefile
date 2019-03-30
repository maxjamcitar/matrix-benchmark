all:	
	gcc matrix.c -o matrix.o -I Dependencies/OpenBLAS/include -LDependencies/OpenBLAS/lib -lopenblas -lm -O2

clean: 
	rm -r matrix.o
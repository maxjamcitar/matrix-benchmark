# matrix-benchmark
Matrix calculation benchmark (multiplication of NxN matrices X times) with FLOPS file output.

Installation and Usage Guide:
1. Compile OpenBLAS library (for your exact system) by executing 'openblas-compile' script. It will take the source code version of OpenBLAS, compile it and install in Dependencies folder. 
2. Compile the benchmark with 'make' command. To remove executable files, run 'make clean'.
3. Now 'matrix' file is created and ready to be run. 1st external argument is the dimension of square matrices, 2nd external argument is an amount of times matrices are multiplied. Example: "./matrix 5 100" will multiply 5x5 matrices 100 times. It will also print FLOPS data in the 'text-output/matrix-output.txt' file.
4. Run 'matrix-script.sh' to perform computational tests. Feel free to change it according to your needs. Timing log will be printed in the 'text-output/matrix-timelog.txt' file.

If you have any questions, email me at mvsusloparov@edu.hse.ru.
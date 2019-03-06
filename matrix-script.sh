#matDims=( 3 5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95 100 )
matDims=100
tries=5
cycles=10000

gcc matrix.c -o matrix.o -lm -O3

#for matDim in "${matDims[@]}"
for ((matDim=1; matDim<$matDims; matDim++))
do
    for (( try=0; try<$tries; try++ ))
    do
        echo "start $(date) $matDim $try" >> matrix-timelog.txt
        echo ""$matDim"x"$matDim" matrix, try $try"
        taskset 0x00000001 ./matrix.o $matDim $cycles
        echo "end $(date) $matDim $try" >> matrix-timelog.txt
    done 
done
#python3 matrix-plot.py matrix-output.txt matrix-plot.png

matDims=( 3 5 10 20 50 75 100 200 300 400 500 600 700 800 900 1000 )
tries=3
cycles=200

for matDim in "${matDims[@]}"
do
    for (( i=0; i<$tries; i++ ))
    do
        echo "start $(date) $matDim $i" >> matrix-timelog.txt
        echo ""$matDim"x"$matDim" matrix, try $i"
        ./matrix-c.o $matDim $cycles
        echo "start $(date) $matDim $i" >> matrix-timelog.txt
    done 
done
#python3 matrix-plot.py matrix-output.txt matrix-plot.png

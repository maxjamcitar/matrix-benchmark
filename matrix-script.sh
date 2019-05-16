matDims=( 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000 2000 )
matDims=100
tries_easy=5
tries_hard=3
cycles=10000
matrixfile=matrix
textfolder=text-output
mode=def

if test ! -f "$matrixfile"
then
    echo "Executable file '$matrixfile' not found (compile it with 'make' command)."
    exit
fi

mkdir -p $textfolder

for (( matDim=1; matDim<$matDims; matDim++ ))
do
    for (( try=0; try<$tries_easy; try++ ))
    do
        echo "start $(date) $matDim $try" >> $textfolder/matrix-timelog.txt
        echo ""$matDim"x"$matDim" matrix, try $try"
        taskset 0x00000001 ./$matrixfile $matDim $cycles $mode # execute only on the 1st core
        echo "end $(date) $matDim $try" >> $textfolder/matrix-timelog.txt
    done 
done

for matDim in "${matDims[@]}"
do
    for (( try=0; try<$tries_hard; try++ ))
    do
        echo "start $(date) $matDim $try" >> $textfolder/matrix-timelog.txt
        echo ""$matDim"x"$matDim" matrix, try $try"
        taskset 0x00000001 ./$matrixfile $matDim $cycles $mode # execute only on the 1st core
        echo "end $(date) $matDim $try" >> $textfolder/matrix-timelog.txt
    done 
done
#python3 matrix-plot.py matrix-output.txt matrix-plot.png
#python3 matrix-plot-log.py matrix-output.txt matrix-plot-log.png

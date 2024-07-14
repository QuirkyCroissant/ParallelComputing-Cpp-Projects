#!/bin/bash


echo "Starting Speed Test for $1 Base line" 

BASELINE=$(./$1 | tail -n 1)

if [ -n "$2" ]; then
    echo "using custom baseline information"
    BASELINE=$2
fi

echo "Baseline perfomance is TIME:" $BASELINE

for THREADS in 2 4 8 16 32 
do
    echo "Starting Speed Test for $1 with $THREADS"

    THREADVALUE=$(srun --nodes=1 ./$1 --num-threads $THREADS | tail -n 1)

    echo  "For $THREADS THREADS"
    echo  "TIME: $THREADVALUE" 
    echo  "SPEEDUP: " $(echo "scale=2; $BASELINE/$THREADVALUE" | bc -l) 
    echo  ""
done
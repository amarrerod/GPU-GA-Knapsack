#!/bin/zsh

for f in $(find /home/amarrero/Proyectos/instances/Uncorrelated -name '*.kp'); do
    filename=$(basename -- "$f")
    extension="${filename##*.}"
    filename="${filename%.*}"
    ./gpu_knapsack -f $f -p 100 -s 100 -x $filename -r 10; 
done

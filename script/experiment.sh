#!/usr/bin/env zsh

pop_size=(16 32 64 128)
cross_rate=(0.6 0.7 0.8 0.9)
max_evals=400000
stats_interval=1000
reps=10
instances='/home/amarrero/Proyectos/instances'
results='/home/amarrero/Proyectos/results_GPU_KP'

for ps in $pop_size;
    do
    for pc in $cross_rate;
        do 
        echo 'Ejecutando el experimento' $ps $pc $max_evals $stats_interval $reps $instances $results
        python runner.py $ps $pc $max_evals $stats_interval $reps $instances $results
    done
done

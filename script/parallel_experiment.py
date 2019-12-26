import shlex
import os
from os import walk
from os.path import isfile, join
import subprocess
from functools import partial
import multiprocessing as mp
from multiprocessing.pool import ThreadPool

GPU_KP = '/home/amarrero/Proyectos/GPU-GA-Knapsack/src/gpu_knapsack'
KP_EXT = '.kp'


def run_experiment(configuration, instance, mut_rate, rep_number):
    print(f'Configuration: {configuration}')
    print(f'Instance: {instance}')
    print(f'Repetition: {rep_number}')
    print(f'Mutation rate: {mut_rate}')
    output_filename = configuration[2] + '/' +  \
        instance.split('/')[6][:-3] + f'_rep_{rep}.rs'
    cmd = GPU_KP + \
        f' {configuration[0]} 400000 {mut_rate} {configuration[1]} 1000 {output_filename} {instance}'
    p = subprocess.Popen(shlex.split(
        cmd), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    return (out, err)


if __name__ == '__main__':
    pop_sizes = [16, 32, 64, 128]
    cross_rates = [0.6, 0.7, 0.8, 0.9]
    max_evals = 400000
    stats = 1000
    reps = 10
    instances = '/home/amarrero/Proyectos/instances/Uncorrelated/'
    results = '/home/amarrero/Proyectos/results_parallel'
    configurations = []
    for psize in pop_sizes:
        for crate in cross_rates:
            cr_str = str(crate).replace('.', '_')
            config_name = f'GPU_KP_PS_{psize}_CR_{cr_str}_{max_evals}_10Reps'
            config_final_dir = f'{results}/{config_name}'
            configurations.append((psize, crate, config_final_dir))
            if not os.path.exists(config_final_dir):
                os.makedirs(config_final_dir)

    pool = ThreadPool(mp.cpu_count())
    results = []
    for root, _, files in walk(instances):
        for file in files:
            file = join(root, file)
            # Nos quedamos con el tamaño de la instancia (N)
            # Y obtenemos la probabilidad de mutacion como 1 / N
            mut_rate = float(1 / (int(file.split('_')[2])))
            for rep in range(reps):
                for config in configurations:
                    results.append(pool.apply_async(
                        run_experiment, (config, file, mut_rate, rep)))
    pool.close()
    pool.join()

    for result in results:
        out, err = result.get()
        print(f'Out: {out}, Err: {err}')

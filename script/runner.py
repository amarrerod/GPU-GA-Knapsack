
from os import walk, mkdir
from os.path import isfile, join, exists
import subprocess
import argparse

GPU_KP = '/home/amarrero/Proyectos/GPU-GA-Knapsack/src/gpu_knapsack'
KP_EXT = '.kp'


def run_experiment(pop_size, cross_rate, evals, stats, reps, instance_path, results_path, verbose=False):
    """
        Ejecutamos el experimento con las instancias
        en el directorio que nos han enviado. Y los resultados 
        dentro del directorio del segundo argumento.
    """
    # Creamos el directorio de salida si no existe
    cr_str = str(cross_rate).replace('.', '-')
    config_name = f'{results_path}/GPU_KP_PS_{pop_size}_CR_{cr_str}_EVALS_{evals}_REPS_{reps}'
    if not exists(config_name):
        mkdir(config_name)

    if verbose:
        print(f'Running experiment with configuration:\n')
        print(f'\t- Popsize: {pop_size}')
        print(f'\t - Crossover rate: {cross_rate}')
        print(f'\t - Mutation rate: 1 / N')
        print(f'\t - Evaluations: {evals}')
        print(f'\t - Repetitions: {reps}')
        print(f'\t - Instances in: {instance_path}')
        print(f'\t - Results in: {config_name}')

    for root, _, files in walk(instance_path):
        for file in files:
            if file.endswith(KP_EXT):
                files.append(join(root, file))
                file = join(root, file)
                # Nos quedamos con el tamaño de la instancia (N)
                # Y obtenemos la probabilidad de mutacion como 1 / N
                mut_rate = float(1 / (int(file.split('_')[2])))
            if verbose:
                print(f'Instance: {file}')
            for rep in range(reps):

                output_filename = config_name + "/" +  \
                    file.split("/")[6][:-3] + f"_rep_{rep}.rs"
                if verbose:
                    print(f'Output file: {output_filename}')

                subprocess.run([GPU_KP, str(pop_size), str(evals),
                                str(mut_rate), str(cross_rate), str(stats), output_filename, file])


if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='GPU Knapsack Runner')
    parser.add_argument('population_size', help='Population size', type=int)
    parser.add_argument('crossover_rate', help='Crossover rate', type=float)
    parser.add_argument('max_evals', help='Number of evaluations', type=int)
    parser.add_argument('stats_interval', type=int)
    parser.add_argument('repetitions', help='Number of repetitions', type=int)
    parser.add_argument('instances', help='Directory of instances')
    parser.add_argument('results', help='Directory of results')

    args = parser.parse_args()

    run_experiment(args.population_size, args.crossover_rate, args.max_evals,
                   args.stats_interval, args.repetitions, args.instances, args.results, verbose=True)

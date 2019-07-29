
import argparse
import subprocess
from os import listdir, walk
from os.path import isfile, join
import numpy as np
import matplotlib.pyplot as plt
from pprint import pprint
from prettytable import PrettyTable

"""
    CONSTANTES
"""
KP_EXT = ".kp"
GPU_KNAPSACK = "/home/amarrero/Proyectos/GPU-GA-Knapsack/src/gpu_knapsack"
MAX_FITNESS = "max_fitness"
AVG_FITNESS = "avg_fitness"
MIN_FITNESS = "min_fitness"
EVALUATIONS = "Evaluations"
FITNESS = "Fitness - Profits"
INSTANCE = "Instance Type"

""""
    Ejecuta un experimento con los ficheros que se
    encuentra dentro del directorio raiz y subdirectorios
    - Repetitions: Repeticiones de cada experimento
"""


def run_experiment(root_path, repetitions):
    for subdirectory, dirs, files in walk(root_path):
        files = [file for file in files if file.endswith(KP_EXT)]
        print(f"Directory: {subdirectory} with {len(files)} files")

        # Ejecutamos cada instancia las repeticiones definidas
        for file in files:
            for rep in range(repetitions):
                print(f"{rep}/{repetitions} of {file}")
                # Creamos los ficheros de resultados para cada repeticion
                output_file = file[:-3] + f"_{rep}"
                full_path_to_file = join(root_path, subdirectory, file)
                subprocess.run(
                    [GPU_KNAPSACK, "-f", f"{full_path_to_file}", "-p", "100", "-s", "100", "-x", f"{output_file}"], check=True)


"""
    Recibe un diccionario donde las claves son las etiquetas de cada representacion
        - data[max]
        - data[avg]
        - data[min]
"""


def plot_results(data, evaluations):
    plt.title("Results from preliminary experiment")
    for tag in data:
        plt.plot(evaluations, data[tag][MAX_FITNESS], label=tag)

    plt.xlabel(EVALUATIONS)
    plt.ylabel(FITNESS)
    plt.legend()
    plt.show()


"""
    Analiza los resultados obtenidos tras multiples repeticiones
    - Paths donde estan los ficheros
    - Patrones de los ficheros a buscar
"""


def merge_results(data):
    number_of_evaluations = len(data[0])
    number_of_repetitions = len(data)
    # Creamos un array donde meteremos la suma de los resultados
    # de las distintas ejecuciones para cada checkpoint
    avg_results = np.zeros((number_of_evaluations))
    for inner_array in data:
        for i in range(number_of_evaluations):
            avg_results[i] += inner_array[i]

    avg_results /= number_of_repetitions
    return avg_results


def analyse_results(paths, patterns):
    print("Analysing the results...")
    results = {}
    evaluations = []
    evaluations_stored = False
    pretty_table = PrettyTable(
        [INSTANCE, MAX_FITNESS, AVG_FITNESS, MIN_FITNESS])
    for path, pattern in zip(paths, patterns):
        print(f"Pattern: {pattern}")
        files = [join(path, file) for file in listdir(path)]
        max_fitness = []
        avg_fitness = []
        min_fitness = []
        for filename in files:
            # Leemos y guardamos todas las columnas
            data = np.genfromtxt(filename, delimiter=" ")
            if len(data) == 3344:
                print(f"File: {filename}")
            max_fitness.append(data[:, 1])
            avg_fitness.append(data[:, 2])
            min_fitness.append(data[:, 3])
            # Guardamos el numero de evaluaciones solo la primera vez
            if not evaluations_stored:
                evaluations = data[:, 0]
                evaluations_stored = True

        results[pattern] = {
            MAX_FITNESS: merge_results(max_fitness),
            AVG_FITNESS: merge_results(avg_fitness),
            MIN_FITNESS: merge_results(min_fitness)
        }
        final_max = results[pattern][MAX_FITNESS][-1]
        final_avg = results[pattern][AVG_FITNESS][-1]
        final_min = results[pattern][MIN_FITNESS][-1]
        pretty_table.add_row([pattern, final_max, final_avg, final_min])

    print(pretty_table)
    return results, evaluations


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Experiment Parallel GA with KP instances")
    # Definimos los posibles argumentos que pueden pasarse al script
    parser.add_argument("--run", action="store_true",
                        help="Run an experiment. Repetitions and Root_dir are required.")
    parser.add_argument("--reps", type=int,
                        help="Number of repetitions for each instance")
    parser.add_argument("--root_dir", type=str,
                        help="Root directory where the instances are stored")

    parser.add_argument("--analyse", action="store_true")

    args = parser.parse_args()
    if args.run:
        if not args.reps and not args.root_dir:
            parser.print_usage()
            raise RuntimeError()

        repetitions = args.reps
        root_dir = args.root_dir
        run_experiment(root_dir, repetitions)

    elif args.analyse:
        pattern = [
            r"50",
            r"100",
            r"500",
            r"1000",
            r"2000"
        ]
        path = [
            "/home/amarrero/Proyectos/GPU-GA-Knapsack/experiment/results/50",
            "/home/amarrero/Proyectos/GPU-GA-Knapsack/experiment/results/100",
            "/home/amarrero/Proyectos/GPU-GA-Knapsack/experiment/results/500",
            "/home/amarrero/Proyectos/GPU-GA-Knapsack/experiment/results/1000",
            "/home/amarrero/Proyectos/GPU-GA-Knapsack/experiment/results/2000"
        ]

        results, evaluations = analyse_results(path, pattern)
        plot_results(results, evaluations)

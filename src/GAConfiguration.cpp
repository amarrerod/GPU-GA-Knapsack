
#include "GAConfiguration.h"

// Mutex para evitar errores de concurrencia al ejecutar el experimento
std::mutex executionMutex;

/**
 * Método de la clase GAConfiguration que ejecuta el algoritmo.
 *  - 1. Carga los parametros de la configuracion del GA.
 *  - 2. Instancia un objeto de la clase TGPU_Evolution.
 *  - 3. Bloquea el acceso a la GPU para evitar condiciones de carrera.
 *  - 4. Ejecuta el algoritmo.
 *  - 5. Obtiene los resultados y libera la GPU para el siguiente hilo.
 *
 *
 **/
void GAConfiguration::run() {
  // Load parameters
  TParameters* Params = TParameters::GetInstance();
  Params->LoadParameters(populationSize, maxEvaluations, mutationRate,
                         crossRate, statsInterval, filename);
  // Create GPU evolution class
  TGPU_Evolution GPU_Evolution(
      false, evoInstance->getNumberOfItems(), evoInstance->getCapacity(),
      evoInstance->getProfits(), evoInstance->getWeights());

  executionMutex.lock();
  unsigned int AlgorithmStartTime;
  AlgorithmStartTime = clock();
  // Run evolution
  runResult = GPU_Evolution.Run();
  unsigned int AlgorithmStopTime = clock();
  runElapsedTime =
      (float)((AlgorithmStopTime - AlgorithmStartTime) / (float)CLOCKS_PER_SEC);
  executionMutex.unlock();
}

/**
 * Constructor de la clase GAConfiguration
 * Define una nueva configuración y ejecuta el algoritmo sobre la GPU
 * en un hilo separado
 **/
GAConfiguration::GAConfiguration(
    const int& popSize, const float& mutRate, const float& crRate,
    const int& maxEvals, const int& statsInterval,
    unique_ptr<EvolutionaryKnapsackInstance> instance, const bool print,
    const string filename)
    : populationSize(popSize),
      maxEvaluations(maxEvals),
      mutationRate(mutRate),
      crossRate(crRate),
      statsInterval(statsInterval),
      printToFile(print),
      filename(filename),
      runElapsedTime(0.0f),
      runResult(0.0f) {
  evoInstance = instance->clone();
  runThread = thread(&GAConfiguration::run, this);
}

/**
 *
 * @param os
 * @param config
 * @return
 */
ostream& operator<<(ostream& os, const GAConfiguration& config) {
  os << "Genetic Algorithm Configuration" << endl;
  os << "Population Size: " << config.populationSize << endl;
  os << "Mutation Rate: " << config.mutationRate << endl;
  os << "Crossover Rate: " << config.crossRate << endl;
  os << "Max Evaluations: " << config.maxEvaluations << endl;
  os << "Max profit found: " << config.runResult << endl;
  os << "Elapsed Time: " << config.runElapsedTime << "s" << endl;
  os << "==================" << endl;
  return os;
}

/**
 * Método que nos permite clonar un individuo de la clase GAConfiguration
 * necesario para trabajar con punteros inteligentes
 **/
unique_ptr<GAConfiguration> GAConfiguration::clone() {
  unique_ptr<GAConfiguration> copy = make_unique<GAConfiguration>();
  copy->populationSize = populationSize;
  copy->mutationRate = mutationRate;
  copy->crossRate = crossRate;
  copy->maxEvaluations = maxEvaluations;
  copy->runResult = runResult;
  copy->runElapsedTime = runElapsedTime;
  copy->statsInterval = statsInterval;
  copy->runThread = move(runThread);
  return copy;
}

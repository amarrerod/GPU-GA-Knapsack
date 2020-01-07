
#include "GAConfiguration.h"

// Mutex para evitar errores de concurrencia al ejecutar el experimento
std::mutex executionMutex;

void GAConfiguration::run() {
  // Load parameters
  TParameters* Params = TParameters::GetInstance();
  Params->LoadParameters(populationSize, maxEvaluations, mutationRate,
                         crossRate, statsInterval, "");
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

GAConfiguration::GAConfiguration(
    const int& popSize, const float& mutRate, const float& crRate,
    const int& maxEvals, const int& statsInterval,
    unique_ptr<EvolutionaryKnapsackInstance> instance)
    : populationSize(popSize),
      maxEvaluations(maxEvals),
      mutationRate(mutRate),
      crossRate(crRate),
      statsInterval(statsInterval),
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
  os << "Run result: " << config.runResult << endl;
  os << "Elapsed Time: " << config.runElapsedTime << "s" << endl;
  os << "==================" << endl;
  return os;
}

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

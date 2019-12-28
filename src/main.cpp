/*
 * File:        main.cpp
 * Author:      Jiri Jaros
 * Affiliation: Brno University of Technology
 *              Faculty of Information Technology
 *
 *              and
 *
 *              The Australian National University
 *              ANU College of Engineering & Computer Science
 *
 * Email:       jarosjir@fit.vutbr.cz
 * Web:         www.fit.vutbr.cz/~jarosjir
 *
 * Comments:    Efficient GPU implementation of the Genetic Algorithm,
 *              solving the Knapsack problem.
 *
 *
 * License:     This source code is distribute under OpenSouce GNU GPL license
 *
 *              If using this code, please consider citation of related papers
 *              at http://www.fit.vutbr.cz/~jarosjir/pubs.php
 *
 *
 *
 * Created on 30 March 2012, 00:00 PM
 */

#include <stdio.h>
#include <array>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
#include "GPU_Evolution.h"
#include "Parameters.h"
using namespace std;

// Mutex para evitar errores de concurrencia al ejecutar el experimento
std::mutex codeMutex;

void experiment(promise<float>&& promiseObject, const int popsize,
                const float mutationRate, const float crossRate,
                const int maxEvals, const int statsInterval,
                const string filename, const string instance) {
  // Load parameters
  TParameters* Params = TParameters::GetInstance();
  Params->LoadParameters(popsize, maxEvals, mutationRate, crossRate,
                         statsInterval, filename, instance);
  // Create GPU evolution class
  TGPU_Evolution GPU_Evolution;
  unsigned int AlgorithmStartTime;
  codeMutex.lock();
  // Run evolution
  float result = GPU_Evolution.Run();
  codeMutex.unlock();
  promiseObject.set_value(result);
}

/*
 * The main function
 *
 * Actualizamos para considerar las repeticiones tambien
 */
int main(int argc, char** argv) {
  const int nPopSizes = 4;
  const int nCrossRates = 4;
  array<int, nPopSizes> popSizes = {16, 32, 64, 128};
  array<float, nCrossRates> crossRates = {0.6f, 0.7f, 0.8f, 0.9f};
  const int statsInterval = 1000;
  const int maxEvals = 200000;
  const string instance =
      "/home/amarrero/Proyectos/instances/Uncorrelated/"
      "Uncorrelated_N_50_R_1000_1.kp";

  vector<future<float>> futures;
  vector<thread> threadPool;
  for (int i = 0; i < nPopSizes; i++) {
    for (int j = 0; j < nCrossRates; j++) {
      string experimentFilename = "testing_experiment_" +
                                  to_string(popSizes[i]) + "_" +
                                  to_string(crossRates[j]) + ".rs";
      promise<float> promiseObject;
      futures.push_back(promiseObject.get_future());
      threadPool.push_back(thread(experiment, move(promiseObject), popSizes[i],
                                  0.05f, crossRates[j], maxEvals, statsInterval,
                                  experimentFilename, instance));
    }
  }
  for (int i = 0; i < threadPool.size(); i++) {
    threadPool[i].join();
  }
  return 0;

}  // end of main
//------------------------------------------------------------------------------

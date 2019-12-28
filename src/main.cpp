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
#include <iostream>

#include "GPU_Evolution.h"
#include "Parameters.h"

using namespace std;

void experiment(const int& popsize, const float& mutationRate,
                const float& crossRate, const int& maxEvals,
                const int& statsInterval, const string& filename,
                const string& instance) {
  // Load parameters
  TParameters* Params = TParameters::GetInstance();
  Params->LoadParameters(popsize, maxEvals, mutationRate, crossRate,
                         statsInterval, filename, instance);
  // Create GPU evolution class
  TGPU_Evolution GPU_Evolution;
  unsigned int AlgorithmStartTime;
  AlgorithmStartTime = clock();
  // Run evolution
  float result = GPU_Evolution.Run();
  unsigned int AlgorithmStopTime = clock();
  cout << "Execution time: "
       << (float)((AlgorithmStopTime - AlgorithmStartTime) /
                  (float)CLOCKS_PER_SEC)
       << " Result: " << result << endl;
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
  experiment(16, 0.05, 0.8, 20000, 1000, "testing_experiment", instance);
  return 0;

}  // end of main
//------------------------------------------------------------------------------

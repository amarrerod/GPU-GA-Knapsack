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
 *
 *
 * Actualizado a 7 de enero de 2020
 * El código genera una instancia del problema de la mochila con unas ciertas
 * caracteristicas. Luego, resuelve dicha instancia para cada una de las
 * configuraciones lanzadas en paralelo.
 *
 *
 *
 */

#include <array>
#include <vector>
#include "GAConfiguration.h"
using namespace std;

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
  // Creamos una instancia de pruebas
  unique_ptr<EvolutionaryKnapsackInstance> instance =
      make_unique<EvolutionaryKnapsackInstance>(5000, 1000);
  cout << "Instance: " << endl << instance << endl;

  vector<GAConfiguration*> configurations;
  for (int i = 0; i < nPopSizes; i++) {
    for (int j = 0; j < nCrossRates; j++) {
      GAConfiguration* config =
          new GAConfiguration(popSizes[i], 0.05f, crossRates[j], maxEvals,
                              statsInterval, instance->clone());
      configurations.push_back(config);
    }
  }
  for (int i = 0; i < configurations.size(); i++) {
    configurations[i]->join();
    cout << *configurations[i] << endl;
  }
  for (int i = 0; i < configurations.size(); i++) {
    delete (configurations[i]);
  }
  return 0;

}  // end of main
//------------------------------------------------------------------------------

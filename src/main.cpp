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
 * Actualizado a 7 de enero de 2020 --> Alejandro Marrero Díaz
 *  -     El código genera una instancia del problema de la mochila con unas
 *        ciertas caracteristicas.
 * -      Luego, resuelve dicha instancia para cada una de las
 *        configuraciones lanzadas en paralelo.
 *
 */

#include <array>
#include <vector>
#include "GAConfiguration.h"
using namespace std;

int main(int argc, char** argv) {
  // Constantes para definir el experimento
  const int nPopSizes = 4;  // Numero de variaciones del parametro popsize
  const int nCrossRates = 4;
  array<int, nPopSizes> popSizes = {16, 32, 64, 128};  // Popsizes
  array<float, nCrossRates> crossRates = {0.6f, 0.7f, 0.8f, 0.9f};
  const int statsInterval = 1000;  // Intervalo para imprimir estadisticas
  const int maxEvals = 200000;     // Evaluaciones a realizar por cada ejecucion
  const int maxReps = 1;
  // Creamos una instancia de pruebas
  const int nItems = 50;
  const int coeffRange = 1000;
  unique_ptr<EvolutionaryKnapsackInstance> instance =
      make_unique<EvolutionaryKnapsackInstance>(nItems, coeffRange);
  cout << "Instancia generada: " << instance << endl;

  vector<GAConfiguration*> configurations;
  for (int i = 0; i < nPopSizes; i++) {
    for (int j = 0; j < nCrossRates; j++) {
      for (int rep = 0; rep < maxReps; rep++) {
        GAConfiguration* config = new GAConfiguration(
            popSizes[i], (1 / (float)nItems), crossRates[j], maxEvals,
            statsInterval, instance->clone(), false, "");
        configurations.push_back(config);
      }
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

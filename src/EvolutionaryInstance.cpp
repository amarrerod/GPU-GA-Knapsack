//
// Created by amarrero on 9/12/19.
//

#include "EvolutionaryInstance.h"
#include <algorithm>

EvolutionaryKnapsackInstance::EvolutionaryKnapsackInstance(int items, int coeff)
    : nItems(items), coefficient(coeff) {
  profits.reserve(nItems);
  weights.reserve(nItems);
  capacity = 0;

  initialize();
  computeCapacity();
}

/**
 *
 * @param individual
 */
void EvolutionaryKnapsackInstance::crossover(
    unique_ptr<EvolutionaryKnapsackInstance> &individual) {
  int crossPoint = Random::get<int>(0, nItems - 1);
  vector<int> tmpProfits(&profits[crossPoint], &profits[nItems]);
  vector<int> tmpWeights(&weights[crossPoint], &weights[nItems]);

  for (int i = 0; i < crossPoint; i++) {
    profits[i] = individual->profits[i];
    weights[i] = individual->weights[i];
  }

  int j = 0;
  for (int i = crossPoint; i < nItems; i++) {
    individual->profits[i] = tmpProfits[j];
    individual->weights[i] = tmpWeights[j];
    j++;
  }
}

/**
 * Operador de mutación --> Mutación Uniforme en cada gen
 */
void EvolutionaryKnapsackInstance::mutation(const float &mutationRate) {
  for (int i = 0; i < nItems; i++) {
    float indexMutationRate = Random::get<float>(0.0, 1.0);
    if (indexMutationRate < mutationRate) {
      profits[i] = Random::get<int>(1, coefficient);
      weights[i] = Random::get<int>(1, coefficient);
    }
  }
}

/**
 * Inicializa una instancia evolutiva del KP
 */
void EvolutionaryKnapsackInstance::initialize() {
  for (int i = 0; i < nItems; i++) {
    weights.emplace_back(
        Random::get<uniform_int_distribution<int>>(1, coefficient));
    profits.emplace_back(
        Random::get<uniform_int_distribution<int>>(1, coefficient));
  }
}

/**
 * Calculamos la capacidad en funcion de la suma de los pesos
 */
void EvolutionaryKnapsackInstance::computeCapacity() {
  int totalWeight = 0;
  for (int &w : weights) totalWeight += w;

  capacity = static_cast<int>(totalWeight * 0.75);
}

/**
 *  Metodo de clonación para trabajar con punteros inteligentes
 * @return
 */
unique_ptr<EvolutionaryKnapsackInstance> EvolutionaryKnapsackInstance::clone() {
  unique_ptr<EvolutionaryKnapsackInstance> copy =
      make_unique<EvolutionaryKnapsackInstance>();
  copy->profits = {this->profits};
  copy->weights = {this->weights};
  copy->nItems = this->nItems;
  copy->coefficient = this->coefficient;
  copy->capacity = this->capacity;
  return copy;
}

/**
 *
 * @param os
 * @param ind
 * @return
 */
ostream &operator<<(ostream &os,
                    unique_ptr<EvolutionaryKnapsackInstance> &ind) {
  os << ind->nItems << endl;
  os << ind->capacity << endl << endl;
  for (int i = 0; i < ind->nItems; i++) {
    os << ind->profits[i] << "\t" << ind->weights[i] << endl;
  }
  return os;
}
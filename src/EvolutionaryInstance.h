
#ifndef GENETIC_GENERATOR_CPP_EVOLUTIONARYINSTANCE_H
#define GENETIC_GENERATOR_CPP_EVOLUTIONARYINSTANCE_H

/**
 * evolutionaryInstance.h
 *
 * Author: Alejandro Marrero
 * Contact: alejandro.marrero.10@ull.edu.es
 *
 *  12 - 2019
 **/
#include <memory>
#include <vector>
#include "random.h"

using namespace std;
using Random = effolkronium::random_static;

/***
 * Clase para representar una instancia del problema de la mochila
 *
 **/
class EvolutionaryKnapsackInstance {
 public:
  EvolutionaryKnapsackInstance() = default;

  EvolutionaryKnapsackInstance(int items, int coeff);

  virtual ~EvolutionaryKnapsackInstance() = default;

  void crossover(unique_ptr<EvolutionaryKnapsackInstance> &individual);

  void mutation(const float &mutationRate);

  void computeCapacity();

  unique_ptr<EvolutionaryKnapsackInstance> clone();

  inline int getNumberOfItems() const { return nItems; };

  inline vector<int> getProfits() const { return profits; };

  inline vector<int> getWeights() const { return weights; };

  inline int getCapacity() const { return capacity; };

  friend ostream &operator<<(ostream &os,
                             unique_ptr<EvolutionaryKnapsackInstance> &ind);

 private:
  void initialize();

 private:
  int nItems;
  int capacity;
  int coefficient;

  vector<int> profits;
  vector<int> weights;
};

#endif
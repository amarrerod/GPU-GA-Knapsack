#pragma once

#ifndef __GACONFIGURATION_H__
#define __GACONFIGURATION_H__
#include <stdio.h>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include "EvolutionaryInstance.h"
#include "GPU_Evolution.h"
#include "Parameters.h"

using namespace std;

class GAConfiguration {
 public:
  GAConfiguration() = default;
  GAConfiguration(const int& popSize, const float& mutationRate,
                  const float& crossRate, const int& maxEvals,
                  const int& statsInterval,
                  unique_ptr<EvolutionaryKnapsackInstance> instance,
                  const bool print, const string filename);
  virtual ~GAConfiguration() = default;

  void run();
  inline void join() { runThread.join(); };
  unique_ptr<GAConfiguration> clone();

 private:
  // Controladores de la ejecucion
  thread runThread = {};
  float runResult;
  float runElapsedTime;
  // Parametros de la configuracion del algoritmo Genetico
  int populationSize;
  int maxEvaluations;
  int statsInterval;
  float mutationRate;
  float crossRate;
  // Flag y nombre del fichero de resultados si necesario
  bool printToFile;
  string filename;
  unique_ptr<EvolutionaryKnapsackInstance> evoInstance;

 public:
  friend ostream& operator<<(ostream& os, const GAConfiguration& ind);
};

#endif
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
                  unique_ptr<EvolutionaryKnapsackInstance> instance);
  virtual ~GAConfiguration() = default;

  void run();
  inline void join() { runThread.join(); };
  unique_ptr<GAConfiguration> clone();

 private:
  // Controladores de la ejecucion
  thread runThread = {};
  future<float> futureObject = {};
  promise<float> promiseObject = {};
  float runResult;
  float runElapsedTime;
  // Parametros de la configuracion del algoritmo Genetico
  int populationSize;
  int maxEvaluations;
  int statsInterval;
  float mutationRate;
  float crossRate;
  unique_ptr<EvolutionaryKnapsackInstance> evoInstance;

 public:
  friend ostream& operator<<(ostream& os, const GAConfiguration& ind);
};

#endif
/*
 * File:        GPU_Evolution.h
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
 * Comments:    Header file of the GA evolution
 *              This class controls the evolution process on multicore CPU
 *
 *
 * License:     This source code is distribute under OpenSouce GNU GPL license
 *
 *              If using this code, please consider citation of related papers
 *              at http://www.fit.vutbr.cz/~jarosjir/pubs.php
 *
 *
 *
 * Created on 24 March 2012, 00:00 PM
 */

#ifndef TGPU_EVOLUTION_H
#define TGPU_EVOLUTION_H

#include <fstream>  // std::ofstream
#include "GPU_Population.h"
#include "GPU_Statistics.h"
#include "GlobalKnapsackData.h"
#include "Parameters.h"
/*
 * struct of seed
 */
struct r123_seed {
  unsigned long seed1;
  unsigned long seed2;
};  // end of r123_seed
//------------------------------------------------------------------------------

/*
 * CPU evolution process
 *
 */
class TGPU_Evolution {
 public:
  // Class constructor
  TGPU_Evolution(bool loadFromFile = true, const int& nItems = 0,
                 const float& capacity = 0.0f, const vector<int>& profits = {},
                 const vector<float>& weights = {});

  // Run evolution
  virtual ~TGPU_Evolution();

  // Run evolution
  float Run();

 protected:
  TParameters* Params;       // Parameters of evolution
  int pActGeneration;        // Actual generation
  int pMultiprocessorCount;  // Number of SM on GPU
  int pDeviceIdx;            // Device ID
  unsigned int pSeed;        // Random Generator Seed
  int performedEvaluations;  // Performed evaluations

  TGPU_Population* MasterPopulation;     // Master GA population
  TGPU_Population* OffspringPopulation;  // Population of offsprings

  TGPU_Statistics* GPUStatistics;  // Statistics over GA process

  TGlobalKnapsackData GlobalData;  // Global data of knapsack

  std::ofstream outputFile;

  // Initialize evolution
  void Initialize();

  // Run evolution
  float RunEvolutionCycle();

  void InitSeed();

  unsigned int GetSeed() { return pSeed++; };

  // Copy constructor not allowed
  TGPU_Evolution(const TGPU_Evolution& orig);
};

#endif /* TGPU_EVOLUTION_H */

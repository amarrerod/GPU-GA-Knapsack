/*
 * File:        CUDA_Kernels.h
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
 * Comments:    Header file of the GA evolution CUDA kernel
 *              This class controls the evolution process on a single GPU
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

#ifndef CUDA_KERNELS_H
#define CUDA_KERNELS_H

#include "GPU_Population.h"
#include "GPU_Statistics.h"
#include "GlobalKnapsackData.h"

/*
 * GPU mutex (binary lock)
 */
class TGPU_Lock {
 public:
  int* mutex;

  TGPU_Lock();
  TGPU_Lock(const TGPU_Lock& orig);
  ~TGPU_Lock();

  __device__ void Lock();
  __device__ void Unlock();

 private:
  long reference_counter;  /// MUST be becacuse I can't pass a class by pointer
                           /// or by reference

};  // end of TGPU_Lock
//------------------------------------------------------------------------------

/// Check and report CUDA errors
void CheckAndReportCudaError(const char* SourceFileName,
                             const int SourceLineNumber);

// First Population generation
__global__ void FirstPopulationGenerationKernel(TPopulationData* PopData,
                                                unsigned int RandomSeed);

// Genetic Manipulation (Selection, Crossover, Mutation)
__global__ void GeneticManipulationKernel(TPopulationData* ParentsData,
                                          TPopulationData* OffspringData,
                                          unsigned int RandomSeed);

// Replacement
__global__ void ReplacementKernel(TPopulationData* ParentsData,
                                  TPopulationData* OffspringData,
                                  unsigned int RandomSeed);

// Calculate statistics
__global__ void CalculateStatistics(TStatisticsData* StatisticsData,
                                    TPopulationData* PopData, TGPU_Lock Lock);

// Calculate OneMax Fitness
__global__ void CalculateFintessOneMax(TPopulationData* PopData);

// Calculate Knapsack fitness
__global__ void CalculateKnapsackFintess(TPopulationData* PopData,
                                         TKnapsackData* GlobalData);

#endif /* CUDA_KERNELS_H */

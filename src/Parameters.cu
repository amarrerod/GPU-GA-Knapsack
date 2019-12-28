/* 
 * File:        Parameters.cu
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
 * Comments:    Implementation file of the parameter class. 
 *              This class maintains all the parameters of evolution.
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
 * Modified on 17 February 2017, 15:59
 */


#include <iostream>
#include <getopt.h>

#include <helper_cuda.h>
#include <cuda_runtime.h>

#include "Parameters.h"

//----------------------------------------------------------------------------//
//                              Definitions                                   //
//----------------------------------------------------------------------------//

__constant__  TEvolutionParameters GPU_EvolutionParameters;


// Singleton initialization 
bool TParameters::pTParametersInstanceFlag = false;
TParameters* TParameters::pTParametersSingle = NULL;


//----------------------------------------------------------------------------//
//                              Implementation                                //
//                              public methods                                //
//----------------------------------------------------------------------------//

/*
 * Get instance of TPrarams
 */
TParameters* TParameters::GetInstance(){
    if(! pTParametersInstanceFlag)
    {        
        pTParametersSingle = new TParameters();
        pTParametersInstanceFlag = true;
        return pTParametersSingle;
    }
    else
    {
        return pTParametersSingle;
    }
}// end of TParameters::GetInstance
//-----------------------------------------------------------------------------

void TParameters::LoadParameters(const int& popsize, const int& maxEvals,
    const float& mutationRate, const float& crossRate,
    const int& statsInterval, const string& filename, const string& instance) {
    float OffspringPercentage = 0.5f;
    EvolutionParameters.PopulationSize = popsize;
    EvolutionParameters.MaxEvaluations = maxEvals;
    EvolutionParameters.MutationPst = mutationRate;
    EvolutionParameters.CrossoverPst = crossRate;
    EvolutionParameters.StatisticsInterval = statsInterval;
    outputFilename = filename;
    GlobalDataFileName = instance;
    // Set population size to be even.
   EvolutionParameters.OffspringPopulationSize = (int) (OffspringPercentage * EvolutionParameters.PopulationSize);
   if (EvolutionParameters.OffspringPopulationSize == 0) EvolutionParameters.OffspringPopulationSize = 2;
   if (EvolutionParameters.OffspringPopulationSize % 2 == 1) EvolutionParameters.OffspringPopulationSize++;
   
      
   // Set UINT mutation threshold to faster comparison
   EvolutionParameters.MutationUINTBoundary  = (unsigned int) ((float) UINT_MAX * EvolutionParameters.MutationPst);
   EvolutionParameters.CrossoverUINTBoundary = (unsigned int) ((float) UINT_MAX * EvolutionParameters.CrossoverPst);
   
}
void TParameters::LoadParameters(const int& popsize, const int& maxEvals,
    const float& mutationRate, const float& crossRate,
    const int& statsInterval, const string& filename) {
    float OffspringPercentage = 0.5f;
    EvolutionParameters.PopulationSize = popsize;
    EvolutionParameters.MaxEvaluations = maxEvals;
    EvolutionParameters.MutationPst = mutationRate;
    EvolutionParameters.CrossoverPst = crossRate;
    EvolutionParameters.StatisticsInterval = statsInterval;
    outputFilename = filename;
    // Set population size to be even.
   EvolutionParameters.OffspringPopulationSize = (int) (OffspringPercentage * EvolutionParameters.PopulationSize);
   if (EvolutionParameters.OffspringPopulationSize == 0) EvolutionParameters.OffspringPopulationSize = 2;
   if (EvolutionParameters.OffspringPopulationSize % 2 == 1) EvolutionParameters.OffspringPopulationSize++;
   
      
   // Set UINT mutation threshold to faster comparison
   EvolutionParameters.MutationUINTBoundary  = (unsigned int) ((float) UINT_MAX * EvolutionParameters.MutationPst);
   EvolutionParameters.CrossoverUINTBoundary = (unsigned int) ((float) UINT_MAX * EvolutionParameters.CrossoverPst);
   
}


/*
 * Load parameters from command line
 * 
 * @param argc
 * @param argv
 * 
 */
void TParameters::LoadParametersFromCommandLine(int argc, char **argv){
    
   // default values
   float OffspringPercentage = 0.5f;
   const int num_args = 8;
   if (argc != num_args) {
       std::cerr << "Error." << std::endl;
       PrintUsageAndExit();
   } else {
       EvolutionParameters.PopulationSize = atoi(argv[1]);
       EvolutionParameters.MaxEvaluations = atoi(argv[2]);
       EvolutionParameters.MutationPst = atof(argv[3]);
       EvolutionParameters.CrossoverPst = atof(argv[4]);
       EvolutionParameters.StatisticsInterval = atoi(argv[5]);
       outputFilename = argv[6];
       GlobalDataFileName = argv[7];
   }

   // Set population size to be even.
   EvolutionParameters.OffspringPopulationSize = (int) (OffspringPercentage * EvolutionParameters.PopulationSize);
   if (EvolutionParameters.OffspringPopulationSize == 0) EvolutionParameters.OffspringPopulationSize = 2;
   if (EvolutionParameters.OffspringPopulationSize % 2 == 1) EvolutionParameters.OffspringPopulationSize++;
   
      
   // Set UINT mutation threshold to faster comparison
   EvolutionParameters.MutationUINTBoundary  = (unsigned int) ((float) UINT_MAX * EvolutionParameters.MutationPst);
   EvolutionParameters.CrossoverUINTBoundary = (unsigned int) ((float) UINT_MAX * EvolutionParameters.CrossoverPst);
   
} // end of LoadParametersFromCommandLine
//------------------------------------------------------------------------------


/*
 * Copy parameters to the GPU constant memory
 */
void TParameters::StoreParamsOnGPU(){
            
    checkCudaErrors(
        cudaMemcpyToSymbol(GPU_EvolutionParameters, &EvolutionParameters, sizeof(TEvolutionParameters) )
    );
    
   
}// end of StoreParamsOnGPU
//------------------------------------------------------------------------------


//----------------------------------------------------------------------------//
//                              Implementation                                //
//                              private methods                               //
//----------------------------------------------------------------------------//

/*
 * Constructor
 */
TParameters::TParameters(){
    
    EvolutionParameters.PopulationSize      = 128;
    EvolutionParameters.ChromosomeSize      = 32;
    // Definido a 400K tras un experimento previo
    EvolutionParameters.MaxEvaluations      = 500000;
        
    EvolutionParameters.MutationPst         = 0.01f;
    EvolutionParameters.CrossoverPst        = 0.7f;    
    EvolutionParameters.OffspringPopulationSize = (int) (0.5f * EvolutionParameters.PopulationSize);
    
    EvolutionParameters.StatisticsInterval  = 1;
    
    EvolutionParameters.IntBlockSize        = sizeof(int)*8;  
    GlobalDataFileName                      = "";
    
    FPrintBest                              = false;
    
    // New parameters
}// end of TParameters
//------------------------------------------------------------------------------

/*
 * print usage of the algorithm
 */
void TParameters::PrintUsageAndExit(){
    
  cerr << "Usage: " << endl;
  cerr << "./gpu_knapsack popsize max_evals mutation_rate cross_rate";
  cerr << " stats_internal output_filename instance_filename" << endl;  

  cerr << endl;
  cerr << "Default Population_size       = 128"  << endl;
  cerr << "Default Number_of_generations = 100" << endl;
  cerr << endl;
  
  cerr << "Default mutation_rate  = 0.01" << endl;
  cerr << "Default crossover_rate = 0.7" << endl;
  cerr << "Default offspring_rate = 0.5" << endl;
  cerr << endl;
  

  cerr << "Default statistics_interval = 1"   << endl;
  
  cerr << "Default benchmark_file_name = knapsack_data.txt\n";
  
  exit(EXIT_FAILURE);
    
}// end of PrintUsage
//------------------------------------------------------------------------------





/*
 * Print all parameters
 * 
 */
void TParameters::PrintAllParameters(){
    printf("-----------------------------------------\n");
    printf("--- Evolution parameters --- \n");
    printf("Population size:     %d\n", EvolutionParameters.PopulationSize);
    printf("Offspring size:      %d\n", EvolutionParameters.OffspringPopulationSize);
    printf("Chromosome int size: %d\n", EvolutionParameters.ChromosomeSize);
    printf("Chromosome size:     %d\n", EvolutionParameters.ChromosomeSize * EvolutionParameters.IntBlockSize);
    
    printf("Max Evaluations:  %d\n", EvolutionParameters.MaxEvaluations);
    printf("\n");
        
    
    printf("Crossover pst:       %f\n", EvolutionParameters.CrossoverPst);
    printf("Mutation  pst:       %f\n", EvolutionParameters.MutationPst);
    printf("Crossover  int:      %u\n",EvolutionParameters.CrossoverUINTBoundary);    
    printf("Mutation  int:       %u\n", EvolutionParameters.MutationUINTBoundary);    
    printf("\n");
    
    printf("Statistics interval: %d\n", EvolutionParameters.StatisticsInterval);
    
    printf("\n");
    printf("Data File: %s\n",GlobalDataFileName.c_str());
    printf("-----------------------------------------\n");
    
    
}// end of PrintAllParameters
//------------------------------------------------------------------------------
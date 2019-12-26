//
// Created by amarrero on 23/12/19.
//
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

void execute(promise<string> &&thread, const char *command) {
  array<char, 128> buffer{};
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  thread.set_value(result);
}

int main(int argc, char **argv) {
  /**
   * Parametros de configuracion del experimento
   * - Numero de configuraciones
   * - Repeticiones
   * - Evaluaciones
   *
   **/
  const int nConfigurations = 4;
  const int nCrossRates = 4;
  int repetitions = 10;
  array<int, nConfigurations> configurationsSizes = {16, 32, 64, 128};
  array<float, nCrossRates> crossRates = {0.6, 0.7, 0.8, 0.9};
  array<string, nCrossRates> crossRatesStr = {"0_6", "0_7", "0_8", "0_9"};
  int nEvaluations = 400000;
  //float mutationRate = 0.01;
  // Definimos el path donde estan las instancias y buscamos todos los ficheros
  const fs::path pathToShow{"/home/amarrero/Proyectos/instances/"};
  auto start = chrono::high_resolution_clock::now();
  for (auto entry = fs::recursive_directory_iterator(pathToShow);
       entry != fs::recursive_directory_iterator(); ++entry) {
    auto filenameStr = entry->path().filename().string();
    if (!entry->is_regular_file()) continue;

    // Calculamos la probabilidad de mutacion a partir del tamaño de la
    // instancia
    filenameStr = filenameStr.substr(0, filenameStr.size() - 3);
    cout << "Filename is: " << filenameStr << endl;
    vector<string> results;
    boost::split(results, filenameStr, [](char c) { return c == '_'; });
    float mutationRate = 1 / stof(results[2]);
    // Para cada fichero lanzamos todas las configuraciones que hemos
    // definido Repitiendo todas las veces que sean necesarias segun el
    // parametro repetitions
    for (int i = 0; i < nConfigurations; i++) {
      for (int j = 0; j < nCrossRates; j++) {
        for (int rep = 0; rep < repetitions; rep++) {
          string command =
              "/home/amarrero/Proyectos/knapsack_instances/GPU-GA-Knapsack/src/"
              "gpu_knapsack " +
              to_string(configurationsSizes[i]) + " 400000 " +
              to_string(mutationRate) + " " + to_string(crossRates[j]) +
              " 1000 PGA_" + to_string(configurationsSizes[i]) + "_" +
              crossRatesStr[j] + "_" + filenameStr + "_" + to_string(rep) +
              ".rs" + " " + entry->path().string();
          promise<string> pThread;
          auto pFuture = pThread.get_future();
          thread configThread(&execute, std::move(pThread), command.c_str());
          configThread.join();
          string results = pFuture.get();
        }
      }
    }
  }
  auto end = chrono::high_resolution_clock::now();
  double elapseTime =
      chrono::duration_cast<chrono::minutes>(end - start).count();
  cout << "Elapsed time is: " << elapseTime << " minutes" << endl;
  return EXIT_SUCCESS;
}

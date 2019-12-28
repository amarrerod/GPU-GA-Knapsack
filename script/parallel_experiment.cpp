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
#include <tuple>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

const string DEFAULT_PATH = "/home/amarrero/Proyectos/instances/";


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
  int nEvaluations = 400000;
  // Creamos un array con las configuraciones de psize y crossRate para hacer
  // mas sencillo el codigo mas abajo
  vector<tuple<int, float>> configurations;
  for (int i = 0; i < nConfigurations; i++) {
    for (int j = 0; j < nCrossRates; j++) {
      configurations.push_back(
          make_tuple(configurationsSizes[i], crossRates[j]));
    }
  }

  // Definimos el path donde estan las instancias y buscamos todos los ficheros
  const fs::path pathToShow{DEFAULT_PATH};
  auto start = chrono::high_resolution_clock::now();
  for (auto entry = fs::recursive_directory_iterator(pathToShow);
       entry != fs::recursive_directory_iterator(); ++entry) {
    auto filenameStr = entry->path().filename().string();
    if (!entry->is_regular_file()) continue;

    // Los resultados de las configuraciones
    // Tenemos los valores max, avg y min promedio de las repeticiones
    vector<tuple<float, float, float>> configurationResults;

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

    for (const tuple<int, float> &config : configurations) {
      tuple<float, float, float> configResults = make_tuple(0.0, 0.0, 0.0);
      for (int rep = 0; rep < repetitions; rep++) {
        string command =
            "/home/amarrero/Proyectos/knapsack_instances/GPU-GA-Knapsack/src/"
            "gpu_knapsack " +
            to_string(get<0>(config)) + " " + to_string(nEvaluations) + " " +
            to_string(mutationRate) + " " + to_string(get<1>(config)) +
            " 1000 PGA_" + to_string(get<0>(config)) + "_" +
            to_string(get<1>(config)) + "_" + filenameStr + "_" +
            to_string(rep) + ".rs" + " " + entry->path().string();
        promise<string> pThread;
        auto pFuture = pThread.get_future();
        thread configThread(&execute, std::move(pThread), command.c_str());
        configThread.join();
        // Nos quedamos con los resultados del proceso y los guardamos en el
        // array
        string results = pFuture.get();
        vector<string> resultsStr;
        boost::split(resultsStr, results, [](char c) { return c == ' '; });
        get<0>(configResults) = stof(resultsStr[0]);
        get<1>(configResults) = stof(resultsStr[1]);
        get<2>(configResults) = stof(resultsStr[2]);
      }
      get<0>(configResults) /= repetitions;
      get<1>(configResults) /= repetitions;
      get<2>(configResults) /= repetitions;
      configurationResults.push_back(configResults);
    }
    // Mostramos la mejor configuracion para la instancia que estamos
    // resolviendo
    sort(configurationResults.begin(), configurationResults.end());
    cout << "Best configuration found get: " << get<0>(configurationResults[0])
         << endl;
  }
  auto end = chrono::high_resolution_clock::now();
  double elapseTime =
      chrono::duration_cast<chrono::minutes>(end - start).count();
  cout << "Elapsed time is: " << elapseTime << " minutes" << endl;
  return EXIT_SUCCESS;
}

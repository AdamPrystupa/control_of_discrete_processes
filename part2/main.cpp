#include "algorithms.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  // if (argc < 4) {
  //   std::cerr << "Usage: " << argv[0]
  //             << " <input_file> <algorithm> <number_of_machines>\n";
  //   return 1;
  // }

  // std::string inputFile = argv[1];
  // std::string algorithm = argv[2];
  // std::string numberOfMachines = argv[3];

  // std::vector<Task> tasks;
  // std::vector<Machine> machines;
  // createMachines(machines, std::stoi(numberOfMachines));

  // int cmax = 0;

  // loadTasksFromFile(inputFile, tasks);
  // std::chrono::duration<double> elapsed;

  // switch (algorithm[0]) {
  // case 'p': {
  //   auto start = std::chrono::high_resolution_clock::now();
  //   lpt(tasks, machines);
  //   auto end = std::chrono::high_resolution_clock::now();
  //   elapsed = end - start;
  //   break;
  // }
  // case 's': {
  //   auto start = std::chrono::high_resolution_clock::now();
  //   lsa(tasks, machines);
  //   auto end = std::chrono::high_resolution_clock::now();
  //   elapsed = end - start;
  //   break;
  // }
  // default:
  //   std::cerr << "Unknown algorithm: " << algorithm << "\n";
  //   return 1;
  // }
  // printTasks(tasks);

  // printMachines(machines);
  // cmax = calculateCmax(machines);

  // std::cout << "Cmax: " << cmax << "\n";
  // std::cout
  //     << "Execution time: "
  //     <<
  //     std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()
  //     << " ns\n";

  std::vector<std::string> algorithms = {"lpt",  "lsa",   "pd",
                                         "ptas", "fptas", "opt"};
  std::chrono::duration<double> elapsed;
  std::string algorithm = "lpt";
  std::string numberOfMachines = "2";

  if (std::find(algorithms.begin(), algorithms.end(), algorithm) ==
      algorithms.end()) {
    std::cerr << "Unknown algorithm: " << algorithm << "\n";
    return 1;
  }
  std::string testDir = "../tests";

  for (const auto &entry : fs::directory_iterator(testDir)) {
    if (!entry.is_regular_file())
      continue;

    std::string testFile = entry.path().string();
    std::cout << "Test file: " << testFile << "\n";

    for (const auto &alg : algorithms) {
      std::vector<Task> tasks;
      std::vector<Machine> machines;
      createMachines(machines, std::stoi(numberOfMachines));
      loadTasksFromFile(testFile, tasks);

      auto start = std::chrono::high_resolution_clock::now();
      if (alg == "lpt") {
        lpt(tasks, machines);
      } else if (alg == "lsa") {
        lsa(tasks, machines);
      } else if (alg == "pd") {
        pd(tasks, machines);
      } else if (alg == "ptas") {
        ptas(tasks, machines);
      } else if (alg == "fptas") {
        fptas(tasks, machines);
      } else if (alg == "opt") {
        opt(tasks, machines);
      } else {
        std::cerr << "Unknown algorithm: " << alg << "\n";
        continue;
      }
      auto end = std::chrono::high_resolution_clock::now();
      elapsed = end - start;

      int cmax = calculateCmax(machines);
      if (alg != "opt") {
        std::cout << "  Algorithm: " << alg << ", Cmax: " << cmax << ", Time: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                         elapsed)
                         .count()
                  << " ns\n";
      } else if (alg == "opt" && tasks.size() < 20) {
        std::cout << ", Time: "
                  << std::chrono::duration_cast<std::chrono::nanoseconds>(
                         elapsed)
                         .count()
                  << " ns\n";
      }
    }
    std::cout << "----------------------------------------\n\n";
  }

  return 0;
}
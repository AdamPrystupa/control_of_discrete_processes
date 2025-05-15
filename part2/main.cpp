#include "algorithms.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> <algorithm> <number_of_machines>\n";
    return 1;
  }

  std::string inputFile = argv[1];
  std::string algorithm = argv[2];
  std::string numberOfMachines = argv[3];

  std::vector<Task> tasks;
  std::vector<Machine> machines;
  createMachines(machines, std::stoi(numberOfMachines));

  int cmax = 0;

  loadTasksFromFile(inputFile, tasks);
  std::chrono::duration<double> elapsed;

  switch (algorithm[0]) {
  case 'p': {
    auto start = std::chrono::high_resolution_clock::now();
    lpt(tasks, machines);
    auto end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    break;
  }
  case 's': {
    auto start = std::chrono::high_resolution_clock::now();
    lsa(tasks, machines);
    auto end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    break;
  }
  default:
    std::cerr << "Unknown algorithm: " << algorithm << "\n";
    return 1;
  }
  printTasks(tasks);

  printMachines(machines);
  cmax = calculateCmax(machines);

  std::cout << "Cmax: " << cmax << "\n";
  std::cout
      << "Execution time: "
      << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()
      << " ns\n";

  return 0;
}
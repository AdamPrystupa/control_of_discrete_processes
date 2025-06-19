#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

// Przegląd zupełny (optymalny) - znajduje permutację z minimalnym Cmax
const int MAX_OPT_TASKS = 11; // Limit tasks for brute-force

struct Task {
  std::vector<int> taskTime;
};

void loadTasksFromFile(const std::string &filename, std::vector<Task> &tasks);
int advanced(const std::vector<Task> &tasks, std::vector<int> &bestPerm);
int calculateCmax(const std::vector<Task> &tasks, const std::vector<int> &perm);
int basic_lb(const std::vector<Task> &tasks, std::vector<int> &bestPerm);
int fneh(const std::vector<Task> &tasks, std::vector<int> &bestPerm);
int johnson(const std::vector<Task> &tasks, std::vector<int> &bestPerm);
int opt(const std::vector<Task> &tasks, std::vector<int> &bestPerm);
int neh(const std::vector<Task> &tasks, std::vector<int> &bestPerm);
void printResult(std::string alg, const std::vector<Task> &tasks, int cmax,
                 const std::chrono::duration<double> &elapsed);

#endif // ALGORITHMS_HPP

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

struct Task {
  int index;
  int pj;
};
struct Machine {
  int index;
  std::vector<Task> tasksToDo;
  int Cmax;
};

void loadTasksFromFile(const std::string &filename, std::vector<Task> &tasks);
void sortTasks(std::vector<Task> &tasks);
int calculateCmax(const std::vector<Machine> &machines);
void printTasks(const std::vector<Task> &tasks);
void lpt(std::vector<Task> &tasks, std::vector<Machine> &machines);
void lsa(std::vector<Task> &tasks, std::vector<Machine> &machines);
void pd(std::vector<Task> &tasks, std::vector<Machine> &machines);
void ptas(std::vector<Task> &tasks, std::vector<Machine> &machines);
void fptas(std::vector<Task> &tasks, std::vector<Machine> &machines);
void opt(std::vector<Task> &tasks, std::vector<Machine> &machines);
void printMachines(const std::vector<Machine> &machines);
void createMachines(std::vector<Machine> &machines, int numberOfMachines);

#endif // ALGORITHMS_HPP

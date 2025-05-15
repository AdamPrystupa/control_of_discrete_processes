#include "algorithms.hpp"

void loadTasksFromFile(const std::string &filename, std::vector<Task> &tasks) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << "\n";
    return;
  }

  int n;
  file >> n;
  tasks.resize(n);

  for (int i = 0; i < n; ++i) {
    tasks[i].index = i + 1;
    file >> tasks[i].pj;
  }

  file.close();
}

void createMachines(std::vector<Machine> &machines, int numberOfMachines) {
  machines.resize(numberOfMachines);
  for (int i = 0; i < numberOfMachines; ++i) {
    machines[i].index = i;
    machines[i].Cmax = 0;
  }
}

void printTasks(const std::vector<Task> &tasks) {
  for (const auto &task : tasks) {
    std::cout << "Task " << task.index << ", pj=" << task.pj << "\n";
  }
}

void sortTasks(std::vector<Task> &tasks) {
  std::sort(tasks.begin(), tasks.end(),
            [](const Task &a, const Task &b) { return a.pj > b.pj; });
}

int calculateCmax(const std::vector<Machine> &machines) {
  int currentTime = 0;
  for (int i = 0; i < machines.size(); i++) {
    if (machines[i].Cmax > currentTime) {
      currentTime = machines[i].Cmax;
    }
  }
  return currentTime; // Ensure 'i' is not used here
}
void printMachines(const std::vector<Machine> &machines) {
  for (int i = 0; i < machines.size(); i++) {
    std::cout << "Machine " << i << ":\n";
    for (auto j : machines[i].tasksToDo) {
      std::cout << "[" << j.index << "] Time:" << j.pj << "\n";
    }
  }
}

void lpt(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  sortTasks(tasks);

  for (const auto &task : tasks) {
    int minCmax = std::numeric_limits<int>::max();
    int minIndex = -1;

    for (size_t i = 0; i < machines.size(); ++i) {
      if (machines[i].Cmax < minCmax) {
        minCmax = machines[i].Cmax;
        minIndex = i;
      }
    }

    machines[minIndex].tasksToDo.push_back(task);
    machines[minIndex].Cmax += task.pj;
  }
}

void lsa(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  for (size_t i = 0; i < tasks.size(); ++i) {
    int machineIndex = i % machines.size();
    machines[machineIndex].tasksToDo.push_back(tasks[i]);
    machines[machineIndex].Cmax += tasks[i].pj;
  }
}

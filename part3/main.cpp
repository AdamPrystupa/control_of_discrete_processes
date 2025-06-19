#include "algorithms.hpp"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct ResultRow {
  std::string testFile;
  std::string algorithm;
  int cmax;
  long long time_us;
};

int main(int argc, char *argv[]) {
  std::vector<std::string> algorithms = {"advanced", "basic+lb", "fneh",
                                         "johnson",  "opt",      "neh"};
  std::string testDir = "../tests";
  std::vector<ResultRow> results;

  for (const auto &entry : fs::directory_iterator(testDir)) {
    if (!entry.is_regular_file())
      continue;
    std::string testFile = entry.path().filename().string();

    for (const auto &alg : algorithms) {
      std::vector<Task> tasks;
      std::vector<int> bestPerm;
      loadTasksFromFile(entry.path().string(), tasks);

      if (tasks.empty()) {
        results.push_back({testFile, alg, -1, -1});
        continue;
      }
      size_t m = tasks[0].taskTime.size();
      bool taskTimeValid = true;
      for (const auto &t : tasks) {
        if (t.taskTime.size() != m) {
          taskTimeValid = false;
          break;
        }
      }
      if (!taskTimeValid) {
        std::cerr << "Invalid taskTime size in file: " << entry.path() << "\n";
        results.push_back({testFile, alg, -1, -1});
        continue;
      }

      int cmax = -1;
      try {
        auto start = std::chrono::high_resolution_clock::now();
        if (alg == "opt") {
          cmax = opt(tasks, bestPerm);
        } else if (alg == "basic+lb") {
          cmax = basic_lb(tasks, bestPerm);
        } else if (alg == "fneh") {
          cmax = fneh(tasks, bestPerm);
        } else if (alg == "johnson") {
          if (m == 2)
            cmax = johnson(tasks, bestPerm);
          else
            cmax = -1;
        } else if (alg == "advanced") {
          cmax = advanced(tasks, bestPerm);
        } else if (alg == "neh") {
          cmax = neh(tasks, bestPerm);
        }
        auto end = std::chrono::high_resolution_clock::now();
        long long time_us =
            std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                .count();
        results.push_back({testFile, alg, cmax, cmax == -1 ? -1 : time_us});
      } catch (const std::exception &e) {
        std::cerr << "Exception in algorithm " << alg << " for file "
                  << entry.path() << ": " << e.what() << "\n";
        results.push_back({testFile, alg, -1, -1});
      } catch (...) {
        std::cerr << "Unknown exception in algorithm " << alg << " for file "
                  << entry.path() << "\n";
        results.push_back({testFile, alg, -1, -1});
      }
    }
  }

  // Wyswielanie wynikow w formie tabeli
  std::cout << std::left << std::setw(25) << "Test file" << std::setw(12)
            << "Algorithm" << std::setw(10) << "Cmax" << std::setw(12)
            << "Time [us]"
            << "\n";
  std::cout << std::string(59, '-') << "\n";
  for (const auto &row : results) {
    std::cout << std::left << std::setw(25) << row.testFile << std::setw(12)
              << row.algorithm << std::setw(10)
              << (row.cmax == -1 ? "N/A" : std::to_string(row.cmax))
              << std::setw(12)
              << (row.time_us == -1 ? "N/A" : std::to_string(row.time_us))
              << "\n";
  }
  return 0;
}
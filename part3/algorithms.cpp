#include "algorithms.hpp"
#include <algorithm> // next_permutation
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>

// Wczytuje zadania z pliku do wektora tasks. Pierwsza liczba to liczba zadań,
// druga to liczba maszyn, potem czasy.
void loadTasksFromFile(const std::string &filename, std::vector<Task> &tasks) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << "\n";
    return;
  }

  int n;
  int m;
  if (!(file >> n)) {
    std::cerr << "Error: Could not read number of tasks from file.\n";
    file.close();
    return;
  }
  if (!(file >> m)) {
    std::cerr << "Error: Could not read number of machines from file.\n";
    file.close();
    return;
  }
  tasks.clear();
  tasks.resize(n);

  for (int i = 0; i < n; ++i) {
    tasks[i].taskTime.resize(m);
    for (int j = 0; j < m; ++j) {
      if (!(file >> tasks[i].taskTime[j])) {
        std::cerr << "Error: Could not read task time for task " << i
                  << ", machine " << j << ".\n";
        file.close();
        return;
      }
    }
  }

  file.close();
}

// Liczy Cmax dla podanej permutacji zadań
int calculateCmax(const std::vector<Task> &tasks,
                  const std::vector<int> &perm) {
  int n = perm.size();
  if (n == 0)
    return 0;
  if (tasks.size() < n) {
    std::cerr << "Error: tasks.size() < perm.size() in calculateCmax\n";
    return -1;
  }
  int m = tasks[0].taskTime.size();
  if (m == 0) {
    std::cerr << "Error: m == 0 in calculateCmax\n";
    return -1;
  }
  for (int idx : perm) {
    if (idx < 0 || idx >= static_cast<int>(tasks.size())) {
      std::cerr << "Error: perm contains invalid index " << idx
                << " (tasks.size()=" << tasks.size() << ")\n";
      return -1;
    }
    if (tasks[idx].taskTime.size() != m) {
      std::cerr << "Error: tasks[" << idx
                << "].taskTime.size() != m in calculateCmax\n";
      return -1;
    }
  }
  std::vector<std::vector<int>> completion(n, std::vector<int>(m, 0));

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      int prevTask = (i > 0) ? completion[i - 1][j] : 0;
      int prevMachine = (j > 0) ? completion[i][j - 1] : 0;
      completion[i][j] =
          std::max(prevTask, prevMachine) + tasks[perm[i]].taskTime[j];
    }
  }
  return completion[n - 1][m - 1];
}

// Przegląd wszystkich permutacji - szuka najlepszej (najmniejszy Cmax)
int opt(const std::vector<Task> &tasks, std::vector<int> &bestPerm) {
  int n = tasks.size();
  if (n > MAX_OPT_TASKS) {
    std::cerr << "Error: Too many tasks for brute-force algorithm (max "
              << MAX_OPT_TASKS << ").\n";
    bestPerm.clear();
    return -1;
  }
  std::vector<int> perm(n);
  for (int i = 0; i < n; ++i)
    perm[i] = i;
  int bestCmax = std::numeric_limits<int>::max();

  do {
    int cmax = calculateCmax(tasks, perm);
    if (cmax < bestCmax) {
      bestCmax = cmax;
      bestPerm = perm;
    }
  } while (std::next_permutation(perm.begin(), perm.end()));

  return bestCmax;
}

// Szybki NEH (FNEH) - wersja z prefixami i suffixami
int fneh(const std::vector<Task> &tasks, std::vector<int> &bestPerm) {
  int n = tasks.size();
  if (n == 0)
    return 0;
  int m = tasks[0].taskTime.size();

  // Liczymy sumę czasów dla każdego zadania
  std::vector<std::pair<int, int>> sumTaskTime(n); // {suma, indeks}
  for (int i = 0; i < n; ++i) {
    int sum = 0;
    for (int j = 0; j < m; ++j)
      sum += tasks[i].taskTime[j];
    sumTaskTime[i] = {sum, i};
  }

  // Sortujemy zadania malejąco po sumie czasów
  std::sort(sumTaskTime.begin(), sumTaskTime.end(),
            [](const auto &a, const auto &b) { return a.first > b.first; });

  // Budujemy permutację wg FNEH
  std::vector<int> perm;
  for (int k = 0; k < n; ++k) {
    int taskIdx = sumTaskTime[k].second;
    int bestCmax = std::numeric_limits<int>::max();
    int bestPos = 0;

    int permSize = perm.size();
    std::vector<std::vector<int>> prefix(permSize + 1, std::vector<int>(m, 0));
    std::vector<std::vector<int>> suffix(permSize + 1, std::vector<int>(m, 0));

    // Liczymy prefixy
    for (int i = 0; i < permSize; ++i) {
      for (int j = 0; j < m; ++j) {
        int prevTask = (i > 0) ? prefix[i][j] : 0;
        int prevMachine = (j > 0) ? prefix[i + 1][j - 1] : 0;
        prefix[i + 1][j] =
            std::max(prevTask, prevMachine) + tasks[perm[i]].taskTime[j];
      }
    }
    // Liczymy suffixy
    for (int i = permSize - 1; i >= 0; --i) {
      for (int j = m - 1; j >= 0; --j) {
        int nextTask = (i + 1 < permSize) ? suffix[permSize - i - 1][j] : 0;
        int nextMachine = (j + 1 < m) ? suffix[permSize - i][j + 1] : 0;
        suffix[permSize - i][j] =
            std::max(nextTask, nextMachine) + tasks[perm[i]].taskTime[j];
      }
    }

    // Wstawiamy nowe zadanie na każdą pozycję i wybieramy najlepszą
    for (int pos = 0; pos <= permSize; ++pos) {
      std::vector<int> c(m, 0);
      if (pos > 0) {
        for (int j = 0; j < m; ++j)
          c[j] = prefix[pos][j];
      }
      for (int j = 0; j < m; ++j) {
        int prevTask = (j > 0) ? c[j - 1] : 0;
        c[j] = std::max(c[j], prevTask) + tasks[taskIdx].taskTime[j];
      }
      if (pos < permSize) {
        for (int j = 0; j < m; ++j) {
          int prev = c[j];
          int suf = suffix[permSize - pos][j];
          c[j] = std::max(prev, suf);
        }
      }
      int cmax = c[m - 1];
      if (cmax < bestCmax) {
        bestCmax = cmax;
        bestPos = pos;
      }
    }
    perm.insert(perm.begin() + bestPos, taskIdx);
  }
  bestPerm = perm;
  return calculateCmax(tasks, bestPerm);
}

// Algorytm Johnsona dla 2 maszyn
int johnson(const std::vector<Task> &tasks, std::vector<int> &bestPerm) {
  int n = tasks.size();
  if (n == 0)
    return 0;
  int m = tasks[0].taskTime.size();
  if (m != 2) {
    std::cerr << "Johnson's algorithm is only applicable for 2 machines.\n";
    bestPerm.clear();
    return -1;
  }

  std::vector<std::tuple<int, int, int>> jobs;
  for (int i = 0; i < n; ++i) {
    jobs.emplace_back(tasks[i].taskTime[0], i, 0);
    jobs.emplace_back(tasks[i].taskTime[1], i, 1);
  }

  std::vector<bool> used(n, false);
  bestPerm.resize(n);
  int left = 0;
  int right = n - 1;

  std::vector<std::pair<int, int>> times(n);
  for (int i = 0; i < n; ++i)
    times[i] = {tasks[i].taskTime[0], tasks[i].taskTime[1]};

  std::vector<int> perm(n);
  int l = 0, r = n - 1;
  std::vector<bool> scheduled(n, false);
  for (int k = 0; k < n; ++k) {
    int minTime = std::numeric_limits<int>::max();
    int minIdx = -1;
    int minMachine = -1;
    for (int i = 0; i < n; ++i) {
      if (scheduled[i])
        continue;
      if (times[i].first < minTime) {
        minTime = times[i].first;
        minIdx = i;
        minMachine = 0;
      }
      if (times[i].second < minTime) {
        minTime = times[i].second;
        minIdx = i;
        minMachine = 1;
      }
    }
    if (minMachine == 0) {
      perm[l++] = minIdx;
    } else {
      perm[r--] = minIdx;
    }
    scheduled[minIdx] = true;
  }
  bestPerm = perm;
  return calculateCmax(tasks, bestPerm);
}

// Placeholder dla basic_lb
int basic_lb(const std::vector<Task> &tasks, std::vector<int> &bestPerm) {
  return 0;
}

// Zaawansowany branch&bound z lepszym LB
int advanced(const std::vector<Task> &tasks, std::vector<int> &bestPerm) {
  int n = tasks.size();
  if (n == 0)
    return 0;
  if (n > MAX_OPT_TASKS) {
    std::cerr << "Error: Too many tasks for brute-force algorithm (max "
              << MAX_OPT_TASKS << ").\n";
    bestPerm.clear();
    return -1;
  }
  int m = tasks[0].taskTime.size();

  int bestCmax = std::numeric_limits<int>::max();
  bestPerm.clear();

  std::vector<int> perm(n);
  for (int i = 0; i < n; ++i)
    perm[i] = i;

  // Rekurencyjnie przeglądamy permutacje, odcinając te z gorszym LB
  std::function<void(int, std::vector<std::vector<int>> &)> dfs =
      [&](int k, std::vector<std::vector<int>> &completion) {
        if (k == n) {
          int cmax = completion[n - 1][m - 1];
          if (cmax < bestCmax) {
            bestCmax = cmax;
            bestPerm = perm;
          }
          return;
        }

        int partialCmax = (k > 0) ? completion[k - 1][m - 1] : 0;

        int lb = partialCmax;
        std::vector<int> minTail(m, 0);
        for (int j = 0; j < m; ++j) {
          int sum = 0;
          for (int i = k; i < n; ++i)
            sum += tasks[perm[i]].taskTime[j];
          minTail[j] = sum;
        }
        int tail = 0;
        for (int j = 0; j < m; ++j)
          tail =
              std::max(tail, (k > 0 ? completion[k - 1][j] : 0) + minTail[j]);
        lb = std::max(lb, tail);

        if (lb >= bestCmax)
          return;

        for (int i = k; i < n; ++i) {
          std::swap(perm[k], perm[i]);
          std::vector<std::vector<int>> newCompletion = completion;
          for (int j = 0; j < m; ++j) {
            int prevTask = (k > 0) ? newCompletion[k - 1][j] : 0;
            int prevMachine = (j > 0) ? newCompletion[k][j - 1] : 0;
            newCompletion[k][j] =
                std::max(prevTask, prevMachine) + tasks[perm[k]].taskTime[j];
          }
          dfs(k + 1, newCompletion);
          std::swap(perm[k], perm[i]);
        }
      };

  std::vector<std::vector<int>> completion(n, std::vector<int>(m, 0));
  dfs(0, completion);
  return bestCmax;
}

// Klasyczny NEH - buduje permutację przez dokładanie zadania na najlepszą
// pozycję
int neh(const std::vector<Task> &tasks, std::vector<int> &bestPerm) {
  int n = tasks.size();
  if (n == 0)
    return 0;
  int m = tasks[0].taskTime.size();
  if (m == 0)
    return 0;

  std::vector<std::pair<int, int>> sumTaskTime(n); // {suma, indeks}
  for (int i = 0; i < n; ++i) {
    int sum = 0;
    for (int j = 0; j < m; ++j)
      sum += tasks[i].taskTime[j];
    sumTaskTime[i] = {sum, i};
  }

  std::sort(sumTaskTime.begin(), sumTaskTime.end(),
            [](const auto &a, const auto &b) { return a.first > b.first; });

  std::vector<int> perm;
  perm.reserve(n);
  for (int k = 0; k < n; ++k) {
    int taskIdx = sumTaskTime[k].second;
    int bestCmax = std::numeric_limits<int>::max();
    int bestPos = 0;
    for (int pos = 0; pos <= static_cast<int>(perm.size()); ++pos) {
      std::vector<int> tempPerm = perm;
      tempPerm.insert(tempPerm.begin() + pos, taskIdx);
      bool valid = true;
      for (int idx : tempPerm) {
        if (idx < 0 || idx >= n) {
          valid = false;
          break;
        }
      }
      if (!valid)
        continue;
      int cmax = 0;
      if (!tempPerm.empty())
        cmax = calculateCmax(tasks, tempPerm);
      if (cmax < bestCmax) {
        bestCmax = cmax;
        bestPos = pos;
      }
    }
    perm.insert(perm.begin() + bestPos, taskIdx);
  }
  bestPerm = perm;
  if (!bestPerm.empty())
    return calculateCmax(tasks, bestPerm);
  return 0;
}

void printResult(std::string alg, const std::vector<Task> &tasks, int cmax,
                 const std::chrono::duration<double> &elapsed) {
  if (alg != "opt") {
    std::cout
        << "  Algorithm: " << alg << ", Cmax: " << cmax << ", Time: "
        << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()
        << " ns\n";
  } else if (alg == "opt" && tasks.size() < 10) {
    std::cout
        << ", Time: "
        << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()
        << " ns\n";
  }
  std::cout << "----------------------------------------\n\n";
}
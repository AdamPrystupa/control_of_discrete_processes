#include "algorithms.hpp"
#include <cmath>

// Wczytuje zadania z pliku tekstowego do wektora tasks. Każdy wiersz pliku
// zawiera czas trwania zadania. Format pliku: pierwsza liczba to liczba zadań,
// kolejne liczby to czasy pj.
void loadTasksFromFile(const std::string &filename, std::vector<Task> &tasks) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << "\n";
    return;
  }

  int n;
  file >> n; // liczba zadań
  tasks.resize(n);

  for (int i = 0; i < n; ++i) {
    tasks[i].index = i + 1;
    file >> tasks[i].pj; // czas trwania zadania
  }

  file.close();
}

// Tworzy wektor maszyn o zadanej liczbie, inicjalizując ich indeksy i Cmax na
// 0.
void createMachines(std::vector<Machine> &machines, int numberOfMachines) {
  machines.resize(numberOfMachines);
  for (int i = 0; i < numberOfMachines; ++i) {
    machines[i].index = i;
    machines[i].Cmax = 0;
  }
}

// Wypisuje na ekran informacje o zadaniach (indeks i czas pj).
void printTasks(const std::vector<Task> &tasks) {
  for (const auto &task : tasks) {
    std::cout << "Task " << task.index << ", pj=" << task.pj << "\n";
  }
}

// Sortuje zadania malejąco według czasu trwania pj (do algorytmu LPT).
void sortTasks(std::vector<Task> &tasks) {
  std::sort(tasks.begin(), tasks.end(),
            [](const Task &a, const Task &b) { return a.pj > b.pj; });
}

// Zwraca największy Cmax spośród wszystkich maszyn (czyli makespan).
int calculateCmax(const std::vector<Machine> &machines) {
  int currentTime = 0;
  for (int i = 0; i < machines.size(); i++) {
    if (machines[i].Cmax > currentTime) {
      currentTime = machines[i].Cmax;
    }
  }
  return currentTime;
}

// Wypisuje na ekran przydział zadań do maszyn oraz ich czasy Cmax.
void printMachines(const std::vector<Machine> &machines) {
  for (int i = 0; i < machines.size(); i++) {
    std::cout << "Machine " << i << ":\n";
    for (auto j : machines[i].tasksToDo) {
      std::cout << "[" << j.index << "] Time:" << j.pj << "\n";
    }
  }
}

// Algorytm LPT (Longest Processing Time first):
// Sortuje zadania malejąco wg pj, a następnie przydziela każde zadanie do
// maszyny o najmniejszym aktualnym Cmax.
void lpt(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  sortTasks(tasks); // sortowanie zadań malejąco

  for (const auto &task : tasks) {
    int minCmax = std::numeric_limits<int>::max();
    int minIndex = -1;

    // Szukamy maszyny o najmniejszym Cmax
    for (size_t i = 0; i < machines.size(); ++i) {
      if (machines[i].Cmax < minCmax) {
        minCmax = machines[i].Cmax;
        minIndex = i;
      }
    }

    // Przydziel zadanie do wybranej maszyny
    machines[minIndex].tasksToDo.push_back(task);
    machines[minIndex].Cmax += task.pj;
  }
}

// Algorytm LSA (List Scheduling Assignment):
// Przydziela zadania do maszyn cyklicznie (round-robin).
void lsa(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  for (size_t i = 0; i < tasks.size(); ++i) {
    int machineIndex = i % machines.size(); // wybór maszyny cyklicznie
    machines[machineIndex].tasksToDo.push_back(tasks[i]);
    machines[machineIndex].Cmax += tasks[i].pj;
  }
}

// Algorytm Permutation-Dynamic (Partition Problem, DP) dla 2 maszyn:
// Znajduje podział zadań na dwa zbiory o sumach jak najbardziej zbliżonych
// (minimalizuje makespan).
void pd(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  // Permutation-Dynamic (Partition Problem, DP) dla 2 maszyn
  if (machines.size() != 2) {
    std::cerr << "PD (Permutation-Dynamic) implemented only for 2 machines!\n";
    return;
  }
  int n = tasks.size();
  int total = 0;
  for (const auto &t : tasks)
    total += t.pj;
  int half = total / 2;

  // dp[s] = true jeśli można uzyskać sumę s z pewnego podzbioru zadań
  std::vector<bool> dp(half + 1, false);
  std::vector<int> prev(half + 1, -1); // do odtworzenia podzbioru
  std::vector<int> from(half + 1, -1);
  dp[0] = true;
  for (int i = 0; i < n; ++i) {
    for (int s = half; s >= tasks[i].pj; --s) {
      if (dp[s - tasks[i].pj] && !dp[s]) {
        dp[s] = true;
        prev[s] = s - tasks[i].pj;
        from[s] = i;
      }
    }
  }
  // znajdź największą możliwą sumę <= half
  int best = half;
  while (best >= 0 && !dp[best])
    --best;

  // Odtwórz podzbiór zadań dla maszyny 0
  std::vector<bool> onMachine0(n, false);
  int s = best;
  while (s > 0) {
    int i = from[s];
    onMachine0[i] = true;
    s = prev[s];
  }
  // Przydziel zadania do maszyn
  for (int i = 0; i < n; ++i) {
    if (onMachine0[i]) {
      machines[0].tasksToDo.push_back(tasks[i]);
      machines[0].Cmax += tasks[i].pj;
    } else {
      machines[1].tasksToDo.push_back(tasks[i]);
      machines[1].Cmax += tasks[i].pj;
    }
  }
}

// Algorytm Permutation-Dynamic (Partition Problem, DP) dla 3 maszyn:
// Znajduje podział zadań na trzy zbiory o sumach jak najbardziej zbliżonych
// (minimalizuje makespan).
void pd3(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  // Permutation-Dynamic (Partition Problem, DP) dla 3 maszyn
  if (machines.size() != 3) {
    std::cerr << "PD3 (Permutation-Dynamic) implemented only for 3 machines!\n";
    return;
  }
  int n = tasks.size();
  int total = 0;
  for (const auto &t : tasks)
    total += t.pj;
  int maxSum = total;
  int half = total / 2;

  // dp[s1][s2] = true jeśli można uzyskać sumy s1 i s2 z pewnego podzbioru
  std::vector<std::vector<bool>> dp(half + 1,
                                    std::vector<bool>(half + 1, false));
  std::vector<std::vector<int>> prev(half + 1, std::vector<int>(half + 1, -1));
  std::vector<std::vector<int>> from(half + 1, std::vector<int>(half + 1, -1));
  dp[0][0] = true;

  // Dynamiczne programowanie po wszystkich zadaniach
  for (int i = 0; i < n; ++i) {
    int pj = tasks[i].pj;
    for (int s1 = half; s1 >= 0; --s1) {
      for (int s2 = half; s2 >= 0; --s2) {
        if (dp[s1][s2]) {
          // Przypisz zadanie do maszyny 0
          if (s1 + pj <= half && !dp[s1 + pj][s2]) {
            dp[s1 + pj][s2] = true;
            prev[s1 + pj][s2] = s1 * (half + 1) + s2;
            from[s1 + pj][s2] = i;
          }
          // Przypisz zadanie do maszyny 1
          if (s2 + pj <= half && !dp[s1][s2 + pj]) {
            dp[s1][s2 + pj] = true;
            prev[s1][s2 + pj] = s1 * (half + 1) + s2;
            from[s1][s2 + pj] = i;
          }
        }
      }
    }
  }

  // Szukamy najlepszego (najbardziej zbalansowanego) podziału
  int bestS1 = 0, bestS2 = 0;
  int bestMax = maxSum;
  for (int s1 = 0; s1 <= half; ++s1) {
    for (int s2 = 0; s2 <= half; ++s2) {
      if (dp[s1][s2]) {
        int s3 = total - s1 - s2;
        int currentMax = std::max({s1, s2, s3});
        if (currentMax < bestMax && s3 >= 0 && s3 <= half) {
          bestMax = currentMax;
          bestS1 = s1;
          bestS2 = s2;
        }
      }
    }
  }

  // Odtwarzanie podziału z tablic pomocniczych
  std::vector<int> assignment(n, 2); // domyślnie wszystko na maszynę 2
  int s1 = bestS1, s2 = bestS2;
  while (s1 != 0 || s2 != 0) {
    int i = from[s1][s2];
    int prevVal = prev[s1][s2];
    int prevS1 = prevVal / (half + 1);
    int prevS2 = prevVal % (half + 1);
    if (s1 != prevS1) {
      assignment[i] = 0; // maszyna 0
      s1 = prevS1;
      s2 = prevS2;
    } else if (s2 != prevS2) {
      assignment[i] = 1; // maszyna 1
      s1 = prevS1;
      s2 = prevS2;
    } else {
      break;
    }
  }

  // Przydziel zadania do maszyn zgodnie z assignment
  for (int i = 0; i < n; ++i) {
    machines[assignment[i]].tasksToDo.push_back(tasks[i]);
    machines[assignment[i]].Cmax += tasks[i].pj;
  }
}

// Algorytm PTAS (Polynomial Time Approximation Scheme) dla 2 maszyn:
// Optymalnie przydziela k największych zadań (przegląd wszystkich podziałów),
// resztę przydziela zachłannie.
void ptas(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  // PTAS dla problemu podziału zadań na 2 maszyny
  if (machines.size() != 2) {
    std::cerr << "PTAS implemented only for 2 machines!\n";
    return;
  }
  double epsilon = 0.2; // możesz zmienić dokładność
  int n = tasks.size();
  int k = static_cast<int>(std::ceil(
      2.0 / epsilon)); // liczba największych zadań do optymalnego przydziału
  if (k > n)
    k = n;

  // Sortuj zadania malejąco wg pj
  std::vector<Task> sortedTasks = tasks;
  std::sort(sortedTasks.begin(), sortedTasks.end(),
            [](const Task &a, const Task &b) { return a.pj > b.pj; });

  // Wygeneruj wszystkie możliwe podziały k największych zadań (2^k podziałów)
  int bestCmax = std::numeric_limits<int>::max();
  std::vector<int> bestAssign(k, 0);
  for (int mask = 0; mask < (1 << k); ++mask) {
    int sum0 = 0, sum1 = 0;
    for (int i = 0; i < k; ++i) {
      if (mask & (1 << i))
        sum0 += sortedTasks[i].pj;
      else
        sum1 += sortedTasks[i].pj;
    }
    int cmax = std::max(sum0, sum1);
    if (cmax < bestCmax) {
      bestCmax = cmax;
      for (int i = 0; i < k; ++i)
        bestAssign[i] = (mask & (1 << i)) ? 0 : 1;
    }
  }
  // Przydziel k największych zadań zgodnie z najlepszym podziałem
  machines[0].tasksToDo.clear();
  machines[0].Cmax = 0;
  machines[1].tasksToDo.clear();
  machines[1].Cmax = 0;
  for (int i = 0; i < k; ++i) {
    machines[bestAssign[i]].tasksToDo.push_back(sortedTasks[i]);
    machines[bestAssign[i]].Cmax += sortedTasks[i].pj;
  }
  // Pozostałe zadania przydziel zachłannie do najmniej obciążonej maszyny
  for (int i = k; i < n; ++i) {
    int idx = (machines[0].Cmax < machines[1].Cmax) ? 0 : 1;
    machines[idx].tasksToDo.push_back(sortedTasks[i]);
    machines[idx].Cmax += sortedTasks[i].pj;
  }
}

// Algorytm FPTAS (Fully Polynomial Time Approximation Scheme) dla 2 maszyn:
// Zeskalowuje czasy zadań, rozwiązuje problem podziału dynamicznie na
// zeskalowanych danych, odtwarza przydział.
void fptas(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  // FPTAS dla problemu podziału zadań na 2 maszyny
  if (machines.size() != 2) {
    std::cerr << "FPTAS implemented only for 2 machines!\n";
    return;
  }
  double epsilon = 0.2; // możesz zmienić dokładność
  int n = tasks.size();
  int total = 0;
  for (const auto &t : tasks)
    total += t.pj;
  int K = (int)(epsilon * total / (2 * n));
  if (K == 0)
    K = 1;
  // Zeskaluj czasy zadań do mniejszych wartości
  std::vector<int> scaled(n);
  for (int i = 0; i < n; ++i)
    scaled[i] = tasks[i].pj / K;
  int scaledSum = 0;
  for (int v : scaled)
    scaledSum += v;
  int half = scaledSum / 2;
  // DP na zeskalowanych danych (Subset Sum)
  std::vector<bool> dp(half + 1, false);
  std::vector<int> prev(half + 1, -1);
  std::vector<int> from(half + 1, -1);
  dp[0] = true;
  for (int i = 0; i < n; ++i) {
    for (int s = half; s >= scaled[i]; --s) {
      if (dp[s - scaled[i]] && !dp[s]) {
        dp[s] = true;
        prev[s] = s - scaled[i];
        from[s] = i;
      }
    }
  }
  int best = half;
  while (best >= 0 && !dp[best])
    --best;
  std::vector<bool> onMachine0(n, false);
  int s = best;
  while (s > 0) {
    int i = from[s];
    onMachine0[i] = true;
    s = prev[s];
  }
  // Przydziel zadania do maszyn zgodnie z wynikiem DP
  machines[0].tasksToDo.clear();
  machines[0].Cmax = 0;
  machines[1].tasksToDo.clear();
  machines[1].Cmax = 0;
  for (int i = 0; i < n; ++i) {
    if (onMachine0[i]) {
      machines[0].tasksToDo.push_back(tasks[i]);
      machines[0].Cmax += tasks[i].pj;
    } else {
      machines[1].tasksToDo.push_back(tasks[i]);
      machines[1].Cmax += tasks[i].pj;
    }
  }
}

// Algorytm optymalny (pełny przegląd permutacji) dla <=20 zadań:
// Przegląda wszystkie możliwe permutacje przydziału zadań do maszyn i wybiera
// najlepszy makespan.
void opt(std::vector<Task> &tasks, std::vector<Machine> &machines) {
  if (tasks.size() >= 20) {
    std::cerr << "OPT algorithm is not suitable for more than 20 tasks.\n";
    return;
  }

  sortTasks(tasks); // sortowanie zadań
  int n = tasks.size();
  int m = machines.size();

  std::vector<int> indices(n);
  for (int i = 0; i < n; ++i) {
    indices[i] = i; // indeksy zadań
  }

  int minCmax = std::numeric_limits<int>::max();

  // Przeglądaj wszystkie permutacje zadań
  do {
    // Wyzeruj maszyny
    for (auto &machine : machines) {
      machine.tasksToDo.clear();
      machine.Cmax = 0;
    }

    // Przydziel zadania do maszyn cyklicznie według permutacji
    for (int i = 0; i < n; ++i) {
      int machineIndex = i % m;
      machines[machineIndex].tasksToDo.push_back(tasks[indices[i]]);
      machines[machineIndex].Cmax += tasks[indices[i]].pj;
    }

    int currentCmax = calculateCmax(machines);

    if (currentCmax < minCmax) {
      minCmax = currentCmax;
    }

  } while (std::next_permutation(indices.begin(), indices.end()));

  std::cout << "  Algorithm: opt, Cmax: " << minCmax;
}
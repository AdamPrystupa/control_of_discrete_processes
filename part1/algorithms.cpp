#include "algorithms.hpp"


void loadTasksFromFile(const std::string& filename, std::vector<Task>& tasks) {
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
        file >> tasks[i].rj >> tasks[i].pj >> tasks[i].qj;
    }

    file.close();
}


void sortByRj(std::vector<Task>& tasks, bool ascending = true) {
    std::sort(tasks.begin(), tasks.end(), [ascending](const Task& a, const Task& b) {
        return ascending ? (a.rj < b.rj) : (a.rj > b.rj);
    });
}


void sortByQj(std::vector<Task>& tasks, bool ascending = true) {
    std::sort(tasks.begin(), tasks.end(), [ascending](const Task& a, const Task& b) {
        return ascending ? (a.qj < b.qj) : (a.qj > b.qj);
    });
}

void optimizeGroupsByCmax(std::vector<Task>& tasks, bool sortByRj) {
    for (size_t i = 0; i < tasks.size();) {
        size_t j = i + 1;
        while (j < tasks.size() && 
               ((sortByRj && tasks[j].rj == tasks[i].rj) || 
                (!sortByRj && tasks[j].qj == tasks[i].qj))) {
            ++j;
        }

        if (j > i + 1) {
            std::vector<Task> group(tasks.begin() + i, tasks.begin() + j);
            std::sort(group.begin(), group.end(), [](const Task& a, const Task& b) {
                std::vector<Task> tempA = {a, b};
                std::vector<Task> tempB = {b, a};
                return calculateCmax(tempA) < calculateCmax(tempB);
            });

            std::copy(group.begin(), group.end(), tasks.begin() + i);
        }

        i = j;
    }
}

int calculateCmax(const std::vector<Task>& tasks) {
    int currentTime = 0;
    int cmax = 0;

    for (const auto& task : tasks) {
        currentTime = std::max(currentTime, task.rj) + task.pj;
        cmax = std::max(cmax, currentTime + task.qj);
    }

    return cmax;
}

void printTasks(const std::vector<Task>& tasks) {
    for (const auto& task : tasks) {
        std::cout << "Task " << task.index << ": rj=" << task.rj << ", pj=" << task.pj << ", qj=" << task.qj << "\n";
    }
}



void findOptimalTaskOrder(std::vector<Task>& tasks) {
    std::vector<Task> bestOrder;
    int bestCmax = std::numeric_limits<int>::max();


    std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.index < b.index;
    });

    do {
        int currentCmax = calculateCmax(tasks);
        if (currentCmax < bestCmax) {
            bestCmax = currentCmax;
            bestOrder = tasks;
        }
    } while (std::next_permutation(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
        return a.index < b.index;
    }));

    tasks = bestOrder;
}




void schrage(std::vector<Task>& tasks) {
    std::vector<Task> G;
    std::vector<Task> N = tasks;
    sortByRj(N);

    int currentTime = 0;
    std::vector<Task> result;

    while (!G.empty() || !N.empty()) {
        while (!N.empty() && N.front().rj <= currentTime) {
            G.push_back(N.front());
            N.erase(N.begin());
        }

        if (G.empty()) {
            currentTime = N.front().rj;
        } else {
            auto it = std::max_element(G.begin(), G.end(), [](const Task& a, const Task& b) {
                return a.qj < b.qj;
            });

            Task task = *it;
            G.erase(it);
            currentTime += task.pj;
            result.push_back(task);
        }
    }

    tasks = result;
}



void schrageWithHeap(std::vector<Task>& tasks) {
    std::vector<Task> G;
    std::vector<Task> N = tasks;
    sortByRj(N);

    int currentTime = 0;

    auto compareQj = [](const Task& a, const Task& b) {
        return a.qj < b.qj;
    };

    std::make_heap(N.begin(), N.end(), compareQj);

    std::vector<Task> result;

    while (!G.empty() || !N.empty()) {
        while (!N.empty() && N.front().rj <= currentTime) {
            G.push_back(N.front());
            std::push_heap(G.begin(), G.end(), compareQj);
            N.erase(N.begin());
            std::make_heap(N.begin(), N.end(), compareQj);
        }

        if (G.empty()) {
            currentTime = N.front().rj;
        } else {
            std::pop_heap(G.begin(), G.end(), compareQj);
            Task task = G.back();
            G.pop_back();
            currentTime += task.pj;
            result.push_back(task);
        }
    }

    tasks = result; 
}

void ownAlgorithm(std::vector<Task>& tasks) {
    std::vector<Task> buffer = tasks; // Tworzenie bufora

    std::sort(buffer.begin(), buffer.end(), [](const Task& a, const Task& b) {
        int sumA = a.rj + a.qj;
        int sumB = b.rj + b.qj;

        if (sumA == sumB) {
            return a.pj < b.pj; // Jeśli sumy są równe, sortuj według pj
        }
        return sumA < sumB; // Sortuj według sumy rj + qj
    });

    tasks = buffer; // Nadpisanie oryginalnego wektora
}

void carlier(std::vector<Task>& tasks) {

}
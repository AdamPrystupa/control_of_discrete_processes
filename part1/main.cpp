#include <iostream>
#include <vector>
#include "algorithms.hpp" 

    #include <string>
    #include <chrono>

    int main(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " <input_file> <algorithm>\n";
            return 1;
        }

        std::string inputFile = argv[1];
        std::string algorithm = argv[2];

        std::vector<Task> tasks;
        int cmax = 0;

        loadTasksFromFile(inputFile, tasks);
        std::chrono::duration<double> elapsed;


        switch (algorithm[0]) {
            case 's':
            {
                auto start = std::chrono::high_resolution_clock::now();
                    schrage(tasks);
                auto end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                break;
            }                        
            case 'h':
            {
                auto start = std::chrono::high_resolution_clock::now();
                    schrageWithHeap(tasks);
                auto end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                break;
            }
            case 'r':
            {
                auto start = std::chrono::high_resolution_clock::now();
                    sortByRj(tasks,true);
                auto end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                break;
            }
            case 'q':
            {
                auto start = std::chrono::high_resolution_clock::now();
                    sortByQj(tasks,true);
                auto end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                break;
            }
            case 'o':
            {
                auto start = std::chrono::high_resolution_clock::now();
                findOptimalTaskOrder(tasks);
                auto end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                break;
            }
            case 'a':
            {
                auto start = std::chrono::high_resolution_clock::now();
                ownAlgorithm(tasks);
                auto end = std::chrono::high_resolution_clock::now();
                elapsed = end - start;
                break;
            }


        
            default:
            std::cerr << "Unknown algorithm: " << algorithm << "\n";
            return 1;
        }



        cmax = calculateCmax(tasks);

        std::cout << "Cmax: " << cmax << "\n";
        std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count() << " ns\n";

        return 0;
    }

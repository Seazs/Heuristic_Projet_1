#include "PFSP.h"

#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <numeric>
#include <random>
#include <algorithm>
#include <chrono>
#include <functional>

#define INT_MAX 2147483647



    PFSP::PFSP(const char filename[]){
        importData(filename);
    }
    // void benchmarkAlgorithm();
    // int computeMakespan();
    // int computeTotalFlowtime();
    void PFSP::importData(const char filename[]){
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return ;
        }

        file >> this->numJobs >> this->numMachines;

        // printf("numJobs: %d\n", this->numJobs);
        // printf("numMachines: %d\n", this->numMachines);

        this->jobs.resize(this->numJobs);
        this->machines.resize(this->numMachines);
        int machine_nbr;
        for (int j = 0; j < this->numJobs; ++j) {
            this->jobs[j].id = j;
            this->jobs[j].processingTimes.resize(this->numMachines);
            for (int i = 0; i < this->numMachines; ++i) {
                file >> machine_nbr;
                file >> this->jobs[j].processingTimes[i];
            }
        }

    }

    std::vector<std::vector<int>> PFSP::computeMakespanTable(const std::vector<int> jobsOrder){



        std::vector<std::vector<int>> makespanTable(this->numJobs, std::vector<int>(this->numMachines, 0));
        // Initialize first machine
        makespanTable[0][0] = this->jobs[jobsOrder[0]].processingTimes[0];
        for (int j = 1; j < jobsOrder.size(); ++j) {
            makespanTable[j][0] = makespanTable[j - 1][0] + this->jobs[jobsOrder[j]].processingTimes[0];
        }
        for (int i = 1; i < this->numMachines; ++i) {
            makespanTable[0][i] = makespanTable[0][i - 1] + this->jobs[jobsOrder[0]].processingTimes[i];
        }
        // Fill the rest of the table
        for (int j = 1; j < jobsOrder.size(); ++j) {
            for (int i = 1; i < this->numMachines; ++i) {
                makespanTable[j][i] = std::max(makespanTable[j-1][i], makespanTable[j][i-1]) + this->jobs[jobsOrder[j]].processingTimes[i];
            }
        }
        return makespanTable;
    }
    void PFSP::updateMakespanTable(std::vector<std::vector<int>>& makespanTable, const std::vector<int>& jobsOrder, int startIndex) {
        for (int j = startIndex; j < this->numJobs; ++j) {
            const auto& processingTimes = this->jobs[jobsOrder[j]].processingTimes;
            makespanTable[j][0] = (j == 0 ? 0 : makespanTable[j - 1][0]) + processingTimes[0];
            for (int i = 1; i < this->numMachines; ++i) {
                makespanTable[j][i] = std::max((j == 0 ? 0 : makespanTable[j - 1][i]), makespanTable[j][i - 1]) + processingTimes[i];
            }
        }
    }

    int PFSP::getMakespan(const std::vector<int> jobsOrder){
        return computeMakespanTable(jobsOrder)[this->numJobs-1][this->numMachines-1];
        
    }
    
    int PFSP::getTotalCompletionTime(const std::vector<int> jobsOrder, const std::vector<std::vector<int>> makespanTable){
        int totalFlowtime = 0;
        for (int j = 0; j < this->numJobs; ++j) {
            totalFlowtime += makespanTable[j][this->numMachines-1];
        }
        return totalFlowtime;
    }
    int PFSP::getTotalCompletionTime(const std::vector<int> jobsOrder){
        std::vector<std::vector<int>> makespanTable = computeMakespanTable(jobsOrder);
        return getTotalCompletionTime(jobsOrder, makespanTable);
    }
    
    // Initial solution generation methods

    std::vector<int> PFSP::generateRandomSolution(){
        std::vector<int> jobsOrder(this->numJobs);
        std::iota(jobsOrder.begin(), jobsOrder.end(), 0); // Fill with 0 to numJobs-1
        std::mt19937 g(std::chrono::steady_clock::now().time_since_epoch().count());
        std::shuffle(jobsOrder.begin(), jobsOrder.end(), g); // Shuffle to create a random permutation
        return jobsOrder;
    }

    std::vector<int> PFSP::simplifiedRZHeuristic() {
        std::vector<int> jobsOrder;

        // Sort jobs by the sum of their processing times
        std::vector<std::pair<int, int>> jobsSumProcessingTimes(this->numJobs);
        for (int j = 0; j < this->numJobs; ++j) {
            jobsSumProcessingTimes[j].first = std::accumulate(this->jobs[j].processingTimes.begin(), this->jobs[j].processingTimes.end(), 0);
            jobsSumProcessingTimes[j].second = j;
            
        }
        std::sort(jobsSumProcessingTimes.begin(), jobsSumProcessingTimes.end());

        
        // Construct the jobs order by adding one job at a time
        for (int step = 0; step < this->numJobs; ++step) {
            int currentJob = jobsSumProcessingTimes[step].second;

            // Test all possible positions for the current job in the current jobsOrder
            std::vector<int> bestOrder;
            int bestMakespan = INT_MAX;
            for (size_t pos = 0; pos <= jobsOrder.size(); ++pos) {
                std::vector<int> tempOrder = jobsOrder;
                tempOrder.insert(tempOrder.begin() + pos, currentJob);
                int currentMakespan = getTotalCompletionTime(tempOrder);
                if (currentMakespan < bestMakespan) {
                    bestMakespan = currentMakespan;
                    bestOrder = tempOrder;
                }
            }

            // Update jobsOrder with the best order found
            jobsOrder = bestOrder;
        }
        
        return jobsOrder;
    }

    void PFSP::transpose(std::vector<int>& jobsOrder, int i, int j) {
        if (std::abs(i - j) == 1) { // Ensure i and j are direct neighbors
            std::swap(jobsOrder[i], jobsOrder[j]);
        }
    }

    void PFSP::exchange(std::vector<int>& jobsOrder, int i, int j) {
        std::swap(jobsOrder[i], jobsOrder[j]);
    }

    void PFSP::insert(std::vector<int>& jobsOrder, int i, int j) {
        int job = jobsOrder[i];
        jobsOrder.erase(jobsOrder.begin() + i);
        if (i < j) {
            jobsOrder.insert(jobsOrder.begin() + (j - 1), job); // Adjust j to account for the shift
        } else {
            jobsOrder.insert(jobsOrder.begin() + j, job);
        }
    }

    std::vector<int> PFSP::iterative_improvement_first(std::vector<int> jobsOrder, const char improvement_method[]) {
        std::string method(improvement_method);
        if (method != "transpose" && method != "exchange" && method != "insert") {
            std::cerr << "Invalid improvement method: " << improvement_method << std::endl;
            return jobsOrder;
        }
    
        std::vector<int> bestOrder = jobsOrder;
        std::vector<std::vector<int>> makespanTable = computeMakespanTable(jobsOrder);
        int bestTCT = getTotalCompletionTime(jobsOrder, makespanTable);
        bool improved = true;
        std::mt19937 g(std::chrono::steady_clock::now().time_since_epoch().count());
        
        // Generate all pairs of indices in random order.
        // this order will be the same for all iterations
        std::vector<std::pair<int, int>> indices;
        for (int i = 0; i < this->numJobs; ++i) {
            for (int j = 0; j < this->numJobs; ++j) {
                if (i != j) {
                    indices.emplace_back(i, j);
                }
            }
        }
        std::shuffle(indices.begin(), indices.end(), g);

        std::function<void(std::vector<int>&, int, int)> neighboor_function;
        if (method == "transpose") {
            neighboor_function = [this](std::vector<int>& jobsOrder, int i, int j) {
                return this->transpose(jobsOrder, i, j);
            };
        } else if (method == "exchange") {
            neighboor_function = [this](std::vector<int>& jobsOrder, int i, int j) {
                return this->exchange(jobsOrder, i, j);
            };
        } else if (method == "insert") {
            neighboor_function = [this](std::vector<int>& jobsOrder, int i, int j) {
                return this->insert(jobsOrder, i, j);
            };
        } else {
            std::cerr << "Invalid improvement method: " << improvement_method << std::endl;
            return jobsOrder;
        }

        while (improved) {
            improved = false;
            std::vector<int> currentOrder = bestOrder;
            std::vector<std::vector<int>> neighborMakespanTable;
        
            for (const auto& [i, j] : indices) {
                std::vector<int> neighborOrder = currentOrder;
                
                neighboor_function(neighborOrder, i, j);
    
                // Create a fresh copy of the makespan table for this neighbor
                neighborMakespanTable = makespanTable;
                updateMakespanTable(neighborMakespanTable, neighborOrder, std::min(i, j));
    
                int neighborTCT = 0;
                for (int k = 0; k < this->numJobs; ++k) {
                    neighborTCT += neighborMakespanTable[k][this->numMachines - 1]; // Sum the last column of the makespan table
                }
    
                if (neighborTCT < bestTCT) {
                    bestTCT = neighborTCT;
                    bestOrder = neighborOrder;
                    makespanTable = neighborMakespanTable; // Update the main makespan table
                    improved = true;
                    
                    break;
                }
            }
        }
        
        this->makespanTable = makespanTable; // Update the main makespan table
        return bestOrder;
    }
    
    std::vector<int> PFSP::iterative_improvement_best(std::vector<int> jobsOrder, const char improvement_method[]) {

        std::string method(improvement_method);
        if (method != "transpose" && method != "exchange" && method != "insert") {
            std::cerr << "Invalid improvement method: " << improvement_method << std::endl;
            return jobsOrder;
        }

        std::vector<int> bestOrder = jobsOrder;
        std::vector<std::vector<int>> makespanTable = computeMakespanTable(jobsOrder);
        int bestTCT = getTotalCompletionTime(jobsOrder, makespanTable);
        bool improved = true;

        std::function<void(std::vector<int>, int, int)> neighboor_function;
        if (method == "transpose") {
            neighboor_function = [this](std::vector<int> jobsOrder, int i, int j) {
                return this->transpose(jobsOrder, i, j);
            };
        } else if (method == "exchange") {
            neighboor_function = [this](std::vector<int> jobsOrder, int i, int j) {
                return this->exchange(jobsOrder, i, j);
            };
        } else if (method == "insert") {
            neighboor_function = [this](std::vector<int> jobsOrder, int i, int j) {
                return this->insert(jobsOrder, i, j);
            };
        }
    
        while (improved) {
            improved = false;
            std::vector<int> currentOrder = bestOrder;
            std::vector<std::vector<int>> neighborMakespanTable;

            for (int i = 0; i < this->numJobs; ++i) {
                for (int j = 0; j < this->numJobs; ++j) {
                    if (i == j) continue;
    
                    std::vector<int> neighborOrder = currentOrder;
    
                    neighboor_function(neighborOrder, i, j);
                    
    
                    // Create a fresh copy of the makespan table for this neighbor
                    neighborMakespanTable = makespanTable;
                    updateMakespanTable(neighborMakespanTable, neighborOrder, std::min(i, j));
    
                    int neighborTCT = 0;
                    for (int k = 0; k < this->numJobs; ++k) {
                        neighborTCT += neighborMakespanTable[k][this->numMachines - 1];
                    }
    
                    if (neighborTCT < bestTCT) {
                        bestTCT = neighborTCT;
                        bestOrder = neighborOrder;
                        improved = true;
                    }
                }
            }
            makespanTable = neighborMakespanTable; // Update the main makespan table
        }
    
        return bestOrder;
    }

    std::vector<int> PFSP::iterative_improvement_algorithm(std::string& improvementType, std::string& neighborhoodType, std::string& initType){
        std::vector<int> initialSolution;

        // Generate initial solution based on initType
        if (initType == "random") {
            initialSolution = generateRandomSolution();
        } else if (initType == "srz") {
            initialSolution = simplifiedRZHeuristic();
        } else {
            std::cerr << "Invalid initType: " << initType << std::endl;
            return std::vector<int>();
        }

        // Apply the iterative improvement method based on improvementType
        if (improvementType == "first") {
            return iterative_improvement_first(initialSolution, neighborhoodType.c_str());
        } else if (improvementType == "best") {
            return iterative_improvement_best(initialSolution, neighborhoodType.c_str());
        } else {
            std::cerr << "Invalid improvementType: " << improvementType << std::endl;
            return std::vector<int>();
        }
    }

    std::vector<int> PFSP::variable_neighborhood_descent_first(std::vector<std::string> neighborhoodOrder) {
        // Start from simplified RZ solution
        std::vector<int> currentSolution = simplifiedRZHeuristic();
        int currentTCT = getTotalCompletionTime(currentSolution);
    
        bool improvement = true;
        while (improvement) {
            improvement = false;
    
            for (size_t i = 0; i < neighborhoodOrder.size(); ++i) {
                std::string nh = neighborhoodOrder[i];
    
                std::vector<int> improvedSolution = iterative_improvement_first(currentSolution, nh.c_str());
                int newTCT = getTotalCompletionTime(improvedSolution, this->makespanTable); // the makespanTable is updated in the iterative_improvement_first function
                if (newTCT < currentTCT) {
                    currentSolution = improvedSolution;
                    currentTCT = newTCT;
                    improvement = true;
                    break;  // restart from first neighborhood
                }
            }
        }
    
        return currentSolution;
    }
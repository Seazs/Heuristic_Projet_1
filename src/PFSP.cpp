#include "PFSP.hpp"
#include "utils.hpp"

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
#include <cmath>

#define INT_MAX 2147483647



    PFSP::PFSP(const char filename[]){
        try {
            importData(filename);
        } catch(const std::runtime_error& e){
            std::cerr << "Error: " << e.what() << std::endl;
            std::cerr << "Please check if the file exists and is accessible." << std::endl;
            exit(EXIT_FAILURE); // Exit gracefully with an error code
        }
        makespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            makespanTable[i] = new int[this->numMachines];
        }
        for (int i = 0; i < this->numJobs; ++i) {
            for (int j = 0; j < this->numMachines; ++j) {
                makespanTable[i][j] = INT_MAX;
            }
        }
    }

    PFSP::~PFSP() {
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] makespanTable[i];
        }
        delete[] makespanTable;
    }
    
    void PFSP::importData(const char filename[]){
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error(std::string("Error opening file: ") + filename);
        }

        
        
        file >> this->numJobs >> this->numMachines;

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

    void PFSP::computeMakespanTable(const std::vector<int>& jobsOrder, int** makespanTable) {

        makespanTable[0][0] = jobs[jobsOrder[0]].processingTimes[0];
        for (int j = 1; j < jobsOrder.size(); ++j) {
            makespanTable[j][0] = makespanTable[j - 1][0] + jobs[jobsOrder[j]].processingTimes[0];
        }
        for (int i = 1; i < numMachines; ++i) {
            makespanTable[0][i] = makespanTable[0][i - 1] + jobs[jobsOrder[0]].processingTimes[i];
        }
        for (int j = 1; j < jobsOrder.size(); ++j) {
            for (int i = 1; i < numMachines; ++i) {
                makespanTable[j][i] = std::max(makespanTable[j - 1][i], makespanTable[j][i - 1]) + jobs[jobsOrder[j]].processingTimes[i];
            }
        }
    }

    
    void PFSP::updateMakespanTable(int** makespanTable, const std::vector<int>& jobsOrder, int startIndex) {
        // Update the makespan table starting from the affected job index
        if (startIndex == 0){
            computeMakespanTable(jobsOrder, makespanTable);
            return;
        }
        
        for (int j = startIndex; j < numJobs; ++j) {
            for (int i = 0; i < numMachines; ++i) {
                if (i == 0){
                    makespanTable[j][i] = jobs[jobsOrder[j-1]].processingTimes[i] + jobs[jobsOrder[j]].processingTimes[i];
                } else{
                    makespanTable[j][i] = std::max(makespanTable[j - 1][i], makespanTable[j][i - 1]) + jobs[jobsOrder[j]].processingTimes[i];
                }
            }
        }
    }

    void PFSP::copyMakespanTable(int** dest, int** src, int rows, int cols) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                dest[i][j] = src[i][j];
            }
        }
    }

    int PFSP::getMakespan(const std::vector<int> jobsOrder){
        int **makespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            makespanTable[i] = new int[this->numMachines];
        }
        computeMakespanTable(jobsOrder, makespanTable);
        int makespan = makespanTable[this->numJobs-1][this->numMachines-1];
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] makespanTable[i];
        }
        delete[] makespanTable;
        return makespan;        
    }
    
    int PFSP::getTotalCompletionTime(const std::vector<int>& jobsOrder, int** makespanTable) {
        int totalFlowtime = 0;
        for (int j = 0; j < numJobs; ++j) {
            totalFlowtime += makespanTable[j][numMachines - 1];
        }
        return totalFlowtime;
    }
    int PFSP::getTotalCompletionTime(const std::vector<int>& jobsOrder){
        int **makespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            makespanTable[i] = new int[this->numMachines];
        }
        computeMakespanTable(jobsOrder, makespanTable);
        int result = getTotalCompletionTime(jobsOrder, makespanTable);
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] makespanTable[i];
        }
        delete[] makespanTable;
        return result;
    }
    
    // Initial solution generation methods

    std::vector<int> PFSP::generateRandomSolution(){
        std::vector<int> jobsOrder(this->numJobs);
        std::iota(jobsOrder.begin(), jobsOrder.end(), 0); // Fill with 0 to numJobs-1
        std::hash<std::string> hasher;
        size_t seed = hasher(filename);
        std::mt19937 g(seed);
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

    std::vector<int> PFSP::iterative_improvement_first(std::vector<int> jobsOrder, std::function<void(std::vector<int>&, int, int)> neighboor_function, std::string neighborhoodType) {
    
        std::vector<int> bestOrder = jobsOrder;
        int** makespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            makespanTable[i] = new int[this->numMachines];
        }
        computeMakespanTable(jobsOrder, makespanTable);
        int bestTCT = getTotalCompletionTime(jobsOrder, makespanTable);
        bool improved = true;
        
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
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 g(seed);
        std::shuffle(indices.begin(), indices.end(), g);

        

        int** neighborMakespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            neighborMakespanTable[i] = new int[this->numMachines];
        }
        

        while (improved) {
            improved = false;
            std::vector<int> currentOrder = bestOrder;
            for (const auto& [i, j] : indices) {
                // check if neighboor function is transpose and if |i - j| < , continue
                if (neighborhoodType == "transpose" && std::abs(i - j) != 1) {
                    continue;
                }
                std::vector<int> neighborOrder = currentOrder;
                
                // Apply the neighbor function to generate a new order
                neighboor_function(neighborOrder, i, j);
    
                // Create a deep copy of the makespan table for this neighbor
                copyMakespanTable(neighborMakespanTable, makespanTable, this->numJobs, this->numMachines);
                // Update the makespan table for the neighbor order
                updateMakespanTable(neighborMakespanTable, neighborOrder, std::min(i, j));
    
                int neighborTCT = getTotalCompletionTime(neighborOrder, neighborMakespanTable);
    
                if (neighborTCT < bestTCT) {
                    bestTCT = neighborTCT;
                    bestOrder = neighborOrder;
                    copyMakespanTable(makespanTable, neighborMakespanTable, this->numJobs, this->numMachines);
                    improved = true;
                    //std::cout << "Improved makespan: " << bestTCT << std::endl;
                    break;
                }
            }
        }
        
        copyMakespanTable(this->makespanTable, makespanTable, this->numJobs, this->numMachines);
        // Clean up
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] neighborMakespanTable[i];
        }
        delete[] neighborMakespanTable;
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] makespanTable[i];
        }
        delete[] makespanTable;
        return bestOrder;
    }
    
    std::vector<int> PFSP::iterative_improvement_best(std::vector<int> jobsOrder, std::function<void(std::vector<int>&, int, int)> neighboor_function, std::string neighborhoodType) {

        std::vector<int> bestOrder = jobsOrder;
        int** makespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            makespanTable[i] = new int[this->numMachines];
        }
        computeMakespanTable(jobsOrder, makespanTable); 
        int bestTCT = getTotalCompletionTime(jobsOrder, makespanTable);
        bool improved = true;

        

        int** neighborMakespanTable = new int*[this->numJobs];
        for (int i = 0; i < this->numJobs; ++i) {
            neighborMakespanTable[i] = new int[this->numMachines];
        }
    
        while (improved) {
            improved = false;
            std::vector<int> currentOrder = bestOrder;
            

            for (int i = 0; i < this->numJobs; ++i) {
                for (int j = 0; j < this->numJobs; ++j) {
                    if (i == j) continue;
                    // check if neighboor function is transpose and if i == j, continue
                    if (neighborhoodType == "transpose" && i == j) {
                        continue;
                    }

                    std::vector<int> neighborOrder = currentOrder;

                    // Apply the neighbor function to generate a new order
                    neighboor_function(neighborOrder, i, j);
                    
                    // Create a fresh copy of the makespan table for this neighbor
                    copyMakespanTable(neighborMakespanTable, makespanTable, this->numJobs, this->numMachines);
                    // Update the makespan table for the neighbor order
                    updateMakespanTable(neighborMakespanTable, neighborOrder, std::min(i, j));
    
                    int neighborTCT = getTotalCompletionTime(neighborOrder, neighborMakespanTable);
    
                    if (neighborTCT < bestTCT) {
                        bestTCT = neighborTCT;
                        bestOrder = neighborOrder;
                        improved = true;
                    }
                }
            }
            copyMakespanTable(makespanTable, neighborMakespanTable, this->numJobs, this->numMachines);
        }
        copyMakespanTable(this->makespanTable, makespanTable, this->numJobs, this->numMachines);
        // Clean up
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] neighborMakespanTable[i];
        }
        delete[] neighborMakespanTable;
        for (int i = 0; i < this->numJobs; ++i) {
            delete[] makespanTable[i];
        }
        delete[] makespanTable;
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



        std::function<void(std::vector<int>&, int, int)> neighboor_function;
        if (neighborhoodType == "transpose") {
            neighboor_function = [this](std::vector<int>& jobsOrder, int i, int j) {
                return this->transpose(jobsOrder, i, j);
            };
        } else if (neighborhoodType == "exchange") {
            neighboor_function = [this](std::vector<int>& jobsOrder, int i, int j) {
                return this->exchange(jobsOrder, i, j);
            };
        } else if (neighborhoodType == "insert") {
            neighboor_function = [this](std::vector<int>& jobsOrder, int i, int j) {
                return this->insert(jobsOrder, i, j);
            };
        } else {
            std::cerr << "Invalid improvement method: " << neighborhoodType << std::endl;
            return initialSolution;
        }

        // Apply the iterative improvement method based on improvementType
        if (improvementType == "first") {
            return iterative_improvement_first(initialSolution, neighboor_function, neighborhoodType);
        } else if (improvementType == "best") {
            return iterative_improvement_best(initialSolution, neighboor_function, neighborhoodType);
        } else {
            std::cerr << "Invalid improvementType: " << improvementType << std::endl;
            return std::vector<int>();
        }
    }

    std::vector<int> PFSP::variable_neighborhood_descent_first(std::vector<int> neighborhoodOrder) {
        // Start from simplified RZ solution
        std::vector<int> currentSolution = simplifiedRZHeuristic();
        int currentTCT = getTotalCompletionTime(currentSolution);

        std::function<void(std::vector<int>&, int, int)> neighborhoodFunctions[3] = {
            [this](std::vector<int>& jobsOrder, int i, int j) { this->transpose(jobsOrder, i, j); },
            [this](std::vector<int>& jobsOrder, int i, int j) { this->exchange(jobsOrder, i, j); },
            [this](std::vector<int>& jobsOrder, int i, int j) { this->insert(jobsOrder, i, j); }
        };
        std::vector<std::string> neighborhoodTypes = {"transpose", "exchange", "insert"};

    
        bool improvement = true;
        while (improvement) {
            improvement = false;
    
            for (size_t i = 0; i < neighborhoodOrder.size(); ++i) {
                std::function<void(std::vector<int>&, int, int)> nh = neighborhoodFunctions[neighborhoodOrder[i]];
    
                std::vector<int> improvedSolution = iterative_improvement_first(currentSolution, nh, neighborhoodTypes[neighborhoodOrder[i]]);
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
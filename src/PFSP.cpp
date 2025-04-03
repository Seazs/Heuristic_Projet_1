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

        printf("numJobs: %d\n", this->numJobs);
        printf("numMachines: %d\n", this->numMachines);

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

    std::vector<std::vector<int>> PFSP::computeMakespanTable(std::vector<int> jobsOrder){

        // check that the jobsorder has the right size and contain all the jobs only once
        // if (jobsOrder.size() != this->numJobs) {
        //     std::cerr << "Jobs order size (" << jobsOrder.size() << ") is not equal to the number of jobs." << std::endl;
        //     return std::vector<std::vector<int>>();
        // }
        // std::vector<int> jobsOrderCopy = jobsOrder;
        // std::sort(jobsOrderCopy.begin(), jobsOrderCopy.end());
        // for (int i = 0; i < this->numJobs; ++i) {
        //     if (jobsOrderCopy[i] != i) {
        //         std::cerr << "Jobs order does not contain all the jobs." << std::endl;
        //         return std::vector<std::vector<int>>();
        //     }
        // }


        std::vector<std::vector<int>> makespanTable(this->numJobs, std::vector<int>(this->numMachines, 0));
        // Initialize first machine
        makespanTable[0][0] = this->jobs[jobsOrder[0]].processingTimes[0];
        for (int j = 1; j < jobsOrder.size(); ++j) {
            makespanTable[j][0] = makespanTable[j-1][0] + this->jobs[jobsOrder[j]].processingTimes[0];
        }
        // Initialize first job
        makespanTable[0][0] = this->jobs[jobsOrder[0]].processingTimes[0];
        for (int i = 1; i < this->numMachines; ++i) {
            makespanTable[0][i] = makespanTable[0][i-1] + this->jobs[jobsOrder[0]].processingTimes[i];
        }
        // Fill the rest of the table
        for (int j = 1; j < jobsOrder.size(); ++j) {
            for (int i = 1; i < this->numMachines; ++i) {
                makespanTable[j][i] = std::max(makespanTable[j-1][i], makespanTable[j][i-1]) + this->jobs[jobsOrder[j]].processingTimes[i];
            }
        }
        return makespanTable;
    }

    int PFSP::getMakespan(std::vector<int> jobsOrder){
        return computeMakespanTable(jobsOrder)[this->numJobs-1][this->numMachines-1];
        
    }
    
    
    // Initial solution generation methods

    std::vector<int> PFSP::generateRandomSolution(){
        std::vector<int> jobsOrder(this->numJobs);
        std::iota(jobsOrder.begin(), jobsOrder.end(), 0); // Fill with 0 to numJobs-1
        std::random_device rd;
        std::mt19937 g(rd());
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
                int currentMakespan = getMakespan(tempOrder);
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

    std::vector<int> PFSP::transpose(std::vector<int> jobsOrder, int i, int j) {
        if (std::abs(i - j) == 1) { // Ensure i and j are direct neighbors
            std::swap(jobsOrder[i], jobsOrder[j]);
        }
        return jobsOrder;
    }

    std::vector<int> PFSP::exchange(std::vector<int> jobsOrder, int i, int j) {
        std::swap(jobsOrder[i], jobsOrder[j]);
        return jobsOrder;
    }

    std::vector<int> PFSP::insert(std::vector<int> jobsOrder, int i, int j) {
        if (i < j) {
            int job = jobsOrder[i];
            jobsOrder.erase(jobsOrder.begin() + i);
            jobsOrder.insert(jobsOrder.begin() + j, job);
        } else {
            int job = jobsOrder[i];
            jobsOrder.erase(jobsOrder.begin() + i);
            jobsOrder.insert(jobsOrder.begin() + j - 1, job);
        }
        return jobsOrder;
    }

    std::vector<int> PFSP::iterative_improvement_first(std::vector<int> jobsOrder, const char improvement_method[]){

        // check that the improvement_methods is either, transpose, exchange or insert
        if (std::string(improvement_method) != "transpose" && 
            std::string(improvement_method) != "exchange" && 
            std::string(improvement_method) != "insert") {
            std::cerr << "Invalid improvement method: " << improvement_method << std::endl;
            return jobsOrder;
        }

        std::vector<int> bestOrder = jobsOrder;
        int bestMakespan = getMakespan(jobsOrder);
        bool improved = true;

        while (improved) {
            improved = false;
            std::vector<int> currentOrder = bestOrder;

            for (int i = 0; i < this->numJobs; ++i) {
                for (int j = 0; j < this->numJobs; ++j) {
                    if (i == j) continue;

                    std::vector<int> neighborOrder;
                    if (std::string(improvement_method) == "transpose" && std::abs(i - j) == 1) {
                        neighborOrder = transpose(currentOrder, i, j);
                    } else if (std::string(improvement_method) == "exchange") {
                        neighborOrder = exchange(currentOrder, i, j);
                    } else if (std::string(improvement_method) == "insert") {
                        neighborOrder = insert(currentOrder, i, j);
                    } else {
                        continue;
                    }

                    int neighborMakespan = getMakespan(neighborOrder);
                    if (neighborMakespan < bestMakespan) {
                        bestMakespan = neighborMakespan;
                        bestOrder = neighborOrder;
                        improved = true;
                    }
                }
            }
        }

        return bestOrder;
    }
    
    std::vector<int> PFSP::iterative_improvement_best(std::vector<int> jobsOrder, const char improvement_method[]){
        std::vector<int> bestOrder = jobsOrder;
        int bestMakespan = getMakespan(jobsOrder);
        bool improved = true;

        while (improved) {
            improved = false;
            std::vector<int> currentOrder = bestOrder;
            int currentBestMakespan = bestMakespan;
            std::vector<int> currentBestOrder = bestOrder;

            for (int i = 0; i < this->numJobs; ++i) {
                for (int j = 0; j < this->numJobs; ++j) {
                    if (i == j) continue;

                    std::vector<int> neighborOrder;
                    if (std::string(improvement_method) == "transpose" && std::abs(i - j) == 1) {
                        neighborOrder = transpose(currentOrder, i, j);
                    } else if (std::string(improvement_method) == "exchange") {
                        neighborOrder = exchange(currentOrder, i, j);
                    } else if (std::string(improvement_method) == "insert") {
                        neighborOrder = insert(currentOrder, i, j);
                    } else {
                        continue;
                    }

                    int neighborMakespan = getMakespan(neighborOrder);
                    if (neighborMakespan < currentBestMakespan) {
                        currentBestMakespan = neighborMakespan;
                        currentBestOrder = neighborOrder;
                        improved = true;
                    }
                }
            }

            if (improved) {
                bestMakespan = currentBestMakespan;
                bestOrder = currentBestOrder;
            }
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
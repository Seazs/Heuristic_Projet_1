#include "algorithm.h"

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
        if (jobsOrder.size() != this->numJobs) {
            std::cerr << "Jobs order size is not equal to the number of jobs." << std::endl;
            return std::vector<std::vector<int>>();
        }
        std::vector<int> jobsOrderCopy = jobsOrder;
        std::sort(jobsOrderCopy.begin(), jobsOrderCopy.end());
        for (int i = 0; i < this->numJobs; ++i) {
            if (jobsOrderCopy[i] != i) {
                std::cerr << "Jobs order does not contain all the jobs." << std::endl;
                return std::vector<std::vector<int>>();
            }
        }

        std::vector<std::vector<int>> makespanTable(this->numJobs, std::vector<int>(this->numMachines, 0));
        // Initialize first machine
        makespanTable[0][0] = this->jobs[jobsOrder[0]].processingTimes[0];
        for (int j = 1; j < this->numJobs; ++j) {
            makespanTable[j][0] = makespanTable[j-1][0] + this->jobs[jobsOrder[j]].processingTimes[0];
        }
        // Initialize first job
        makespanTable[0][0] = this->jobs[jobsOrder[0]].processingTimes[0];
        for (int i = 1; i < this->numMachines; ++i) {
            makespanTable[0][i] = makespanTable[0][i-1] + this->jobs[jobsOrder[0]].processingTimes[i];
        }
        // Fill the rest of the table
        for (int j = 1; j < this->numJobs; ++j) {
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
    




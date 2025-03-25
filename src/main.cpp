#include <iostream>
#include "algorithm.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

std::vector<std::vector<int>> importData(const char filename[]);


int main() {
    // Initialize the algorithm
    std::cout << "Starting algorithm benchmarking..." << std::endl;

    // Import the data
    std::vector<std::vector<int>> processingTimes = importData("./res/Benchmarks/ta052");

    // Call the algorithm function
    std::cout << "Benchmarking completed." << std::endl;

    int size = sizeof(int);
    std::cout << "Size of int: " << size << std::endl;
    return 0;
}


std::vector<std::vector<int>> importData(const char filename[]) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }

    int numJobs, numMachines;
    file >> numJobs >> numMachines;

    std::vector<std::vector<int>> processingTimes(numJobs, std::vector<int>(numMachines));
    int machine_nbr;
    for (int j = 0; j < numJobs; ++j) {
        for (int i = 0; i < numMachines; ++i) {
            file >> machine_nbr;
            file >> processingTimes[j][i];
        }
    }

    file.close();
    return processingTimes;
}


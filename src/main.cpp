#include <iostream>
#include "algorithm.h"
#include <algorithm>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <numeric>
#include <random>


using namespace std;



int main() {
    PFSP pfsp("./data/Benchmarks/ta01");
    // std::vector<int> jobsOrder(50);
    // std::iota(jobsOrder.begin(), jobsOrder.end(), 0); // Fill with 0 to 49
    // std::random_device rd;
    // std::mt19937 g(rd());
    // std::shuffle(jobsOrder.begin(), jobsOrder.end(), g); // Shuffle to create a random permutation

    std::vector<int> jobsOrder = {0, 1, 2, 3, 4, 5};

    pfsp.makespanTable = pfsp.computeMakespanTable(jobsOrder);

    for(int i = 0; i < pfsp.numMachines; i++){
        for(int j = 0; j < pfsp.numJobs; j++){
            cout << pfsp.makespanTable[j][i] << " ";
        }
        cout << endl;
    }

    return 0;
}





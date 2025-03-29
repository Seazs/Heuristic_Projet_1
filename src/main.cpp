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
    std::vector<int> jobsOrder = pfsp.generateRandomSolution(); 
    printf("Random solution: ");
    for (int i = 0; i < jobsOrder.size(); ++i) {
        printf("%d ", jobsOrder[i]);
    }
    printf("\n");
    std::cout << "Random solution makespan: " << pfsp.getMakespan(jobsOrder) << std::endl;
    jobsOrder = pfsp.simplifiedRZHeuristic();
    printf("Simplified RZ heuristic solution: ");
    for (int i = 0; i < jobsOrder.size(); ++i) {
        printf("%d ", jobsOrder[i]);
    }
    printf("\n");
    std::cout << "RZ heuristic makespan: " << pfsp.getMakespan(jobsOrder) << std::endl;
    

    return 0;
}





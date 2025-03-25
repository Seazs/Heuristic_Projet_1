#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>



// Function declarations for the algorithm
void exampleAlgorithm(int n);
void benchmarkAlgorithm();

int computeMakespan(const std::vector<std::vector<int>>& processingTimes, const std::vector<int>& jobOrder);
int computeTotalFlowtime(const std::vector<std::vector<int>>& processingTimes, const std::vector<int>& jobOrder);   

#endif // ALGORITHM_H
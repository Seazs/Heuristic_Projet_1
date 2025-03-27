#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <vector>




struct Job {
    int id;
    std::vector<int> processingTimes;
};

struct Machine {
    std::vector<int> jobOrder;
    int currentTime;
};

class PFSP {
public:
    PFSP(const char filename[]);
    void benchmarkAlgorithm();
    int getMakespan(std::vector<int> jobsOrder);
    std::vector<std::vector<int>> computeMakespanTable(std::vector<int> jobsOrder);
    int computeTotalFlowtime();
    void importData(const char filename[]);

    // initial solution generation
    std::vector<int> generateRandomSolution();
    std::vector<int> simplifiedRZHeuristic();

    std::vector<Job> jobs;
    std::vector<Machine> machines;
    std::vector<std::vector<int>> makespanTable;
    int numMachines;
    int numJobs;

private:
    

    int makespan;
    int totalFlowtime;
};


#endif
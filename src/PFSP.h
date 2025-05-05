#ifndef PFSP_H
#define PFSP_H

#include <vector>
#include <string>



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
    int getMakespan(std::vector<int> jobsOrder);
    std::vector<std::vector<int>> computeMakespanTable(std::vector<int> jobsOrder);
    void updateMakespanTable(std::vector<std::vector<int>>& makespanTable, const std::vector<int>& jobsOrder, int startIndex);
    int getTotalCompletionTime(std::vector<int> jobsOrder);


    // initial solution generation
    std::vector<int> generateRandomSolution();
    std::vector<int> simplifiedRZHeuristic();

    // improvement methods
    std::vector<int> transpose(std::vector<int> jobsOrder, int i, int j);
    std::vector<int> exchange(std::vector<int> jobsOrder, int i, int j);
    std::vector<int> insert(std::vector<int> jobsOrder, int i, int j);

    // iterative methods
    std::vector<int> iterative_improvement_first(std::vector<int> jobsOrder, const char improvement_method[]);
    std::vector<int> iterative_improvement_best(std::vector<int> jobsOrder, const char improvement_method[]);

    // main algo method
    std::vector<int> iterative_improvement_algorithm(std::string& improvementType, std::string& neighborhoodType, std::string& initType);

    // VND method
    std::vector<int> variable_neighborhood_descent_first(std::vector<std::string> neighborhoodOrder);

    void importData(const char filename[]);


    std::vector<Job> jobs;
    std::vector<Machine> machines;
    std::vector<std::vector<int>> makespanTable;
    unsigned int numMachines;
    unsigned int numJobs;

private:
    

    int makespan;
    int totalFlowtime;
};


#endif
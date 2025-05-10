#include "PFSP.h"
// #include "utils.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <numeric>
#include <random>


using namespace std;

void handleProgramParameters(int argc, char* argv[], std::string& dataFileName, std::string& improvementType, std::string& neighborhoodType, std::string& initType);

int main(int argc, char* argv[]) {

    
    std::string dataFileName, improvementType, neighborhoodType, initType;
    
    handleProgramParameters(argc, argv, dataFileName, improvementType, neighborhoodType, initType);

    PFSP pfsp(dataFileName.c_str());

    std::vector<int> algo_result_jobs_order ;
    
    if (neighborhoodType == "vnd1") {
        std::vector<std::string> neighborhoodOrder = {"transpose", "exchange", "insert"};
        algo_result_jobs_order = pfsp.variable_neighborhood_descent_first(neighborhoodOrder);
    } else if (neighborhoodType == "vnd2") {
        std::vector<std::string> neighborhoodOrder = {"transpose", "insert", "exchange"};
        algo_result_jobs_order = pfsp.variable_neighborhood_descent_first(neighborhoodOrder);
    } else {
        algo_result_jobs_order = pfsp.iterative_improvement_algorithm(improvementType, neighborhoodType, initType);
    }

    // print_jobs_order(algo_result_jobs_order);
    int TotalCompletionTime = pfsp.getTotalCompletionTime(algo_result_jobs_order);
    printf("%d", TotalCompletionTime);

    return 0;
}

void handleProgramParameters(int argc, char* argv[], std::string& dataFileName, std::string& improvementType, std::string& neighborhoodType, std::string& initType) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <data-file> --<first|best> --<transpose|exchange|insert> --<srz|random>" << std::endl;
        std::cerr << "Example: ./Heuristic_PSFP data.txt --first --transpose --random" << std::endl;
        exit(EXIT_FAILURE);
    }

    dataFileName = argv[1];
    improvementType = argv[2];
    neighborhoodType = argv[3];
    initType = argv[4];

    if (improvementType != "--first" && improvementType != "--best") {
        std::cerr << "Invalid improvement type. Must be '--first' or '--best'." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (neighborhoodType != "--transpose" && neighborhoodType != "--exchange" && neighborhoodType != "--insert" &&
        neighborhoodType != "--vnd1" && neighborhoodType != "--vnd2") {
        std::cerr << "Invalid neighborhood type. Must be '--transpose', '--exchange', '--insert', '--vnd1', or '--vnd2'." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (initType != "--srz" && initType != "--random") {
        std::cerr << "Invalid initialization type. Must be '--srz' or '--random'." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Remove the leading '--' from the parameters
    improvementType = improvementType.substr(2);
    neighborhoodType = neighborhoodType.substr(2);
    initType = initType.substr(2);
}





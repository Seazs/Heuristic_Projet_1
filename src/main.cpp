#include "PFSP.h"
#include "utils.h"

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


    std::vector<int> algo_result_jobs_order = pfsp.iterative_improvement_algorithm(improvementType, neighborhoodType, initType);

    print_jobs_order(algo_result_jobs_order);
    printf("computed makespan: %d\n", pfsp.getMakespan(algo_result_jobs_order));

    return 0;
}

void handleProgramParameters(int argc, char* argv[], std::string& dataFileName, std::string& improvementType, std::string& neighborhoodType, std::string& initType) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <data-file> <program-parameters>" << std::endl;
        std::cerr << "Example: pfsp data.txt ii--first--transpose--srz" << std::endl;
        exit(EXIT_FAILURE);
    }

    dataFileName = argv[1];
    std::string params = argv[2];
    std::istringstream ss(params);
    std::vector<std::string> tokens;
    std::string token;

    while (std::getline(ss, token, '-')) {
        if (!token.empty() && token != "ii") {
            tokens.push_back(token);
        }
    }

    if (tokens.size() != 3) {
        std::cerr << "Invalid parameter format. Expected format: ii--<first|best>--<transpose|exchange|insert>--<srz|random>" << std::endl;
        exit(EXIT_FAILURE);
    }

    improvementType = tokens[0];
    neighborhoodType = tokens[1];
    initType = tokens[2];

    if (improvementType != "first" && improvementType != "best") {
        std::cerr << "Invalid improvement type. Must be 'first' or 'best'." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (neighborhoodType != "transpose" && neighborhoodType != "exchange" && neighborhoodType != "insert") {
        std::cerr << "Invalid neighborhood type. Must be 'transpose', 'exchange', or 'insert'." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (initType != "srz" && initType != "random") {
        std::cerr << "Invalid initialization type. Must be 'srz' or 'random-init'." << std::endl;
        exit(EXIT_FAILURE);
    }
}





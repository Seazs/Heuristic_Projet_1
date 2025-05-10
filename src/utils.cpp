#include <vector>
#include <string>

void print_jobs_order(std::vector<int> jobsOrder){
    for (int i = 0; i < jobsOrder.size(); ++i) {
        printf("%d ", jobsOrder[i]);
    }
    printf("\n");
}

void print_makespan_table(std::vector<std::vector<int>> makespanTable){
    for (int j = 0; j < makespanTable[0].size(); ++j) {
        for (int i = 0; i < makespanTable.size(); ++i) {
            printf("%d ", makespanTable[i][j]);
        }
        printf("\n");
    }
}

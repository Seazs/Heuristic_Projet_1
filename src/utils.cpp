#include <vector>
#include <string>

void print_jobs_order(std::vector<int> jobsOrder){
    for (int i = 0; i < jobsOrder.size(); ++i) {
        printf("%d ", jobsOrder[i]);
    }
    printf("\n");
}

void print_makespan_table(int** makespanTable, int rows, int cols){
    for (int j = 0; j < cols; ++j) {
        for (int i = 0; i < rows; ++i) {
            printf("%d ", makespanTable[i][j]);
        }
        printf("\n");
    }
}

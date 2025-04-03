#include <vector>
#include <string>

void print_jobs_order(std::vector<int> jobsOrder){
    for (int i = 0; i < jobsOrder.size(); ++i) {
        printf("%d ", jobsOrder[i]);
    }
    printf("\n");
}

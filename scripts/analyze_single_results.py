import pandas as pd
from scipy.stats import wilcoxon

# Load the detailed results
data = pd.read_csv("detailed_result_VND.csv")

# Function to perform Wilcoxon test and print results
def perform_wilcoxon_test(group1, group2, metric, description):
    stat, p_value = wilcoxon(group1, group2)
    print(f"{description}")
    print(f"Wilcoxon test statistic: {stat}, p-value: {p_value}")
    if p_value < 0.05:
        print("=> Statistically significant difference")
        if group1.mean() < group2.mean():
            print("Group 1 has better performance on average.")
        else:
            print("Group 2 has better performance on average.")
        print("\n")
    else:
        print("=> No statistically significant difference\n")

# (i) Which initial solution is preferable?
print("Question (i): Which initial solution is preferable?")
pivoting_rules = ["best", "first"]
neighborhoods = ["transpose", "exchange", "insert"]
initial_solutions = ["random", "srz"]

selection_strategies = ["strategy1", "strategy2"]

for pivoting_rule in pivoting_rules:
    for neighborhood in neighborhoods:
        subset = data[(data["Pivoting Rule"] == pivoting_rule) & (data["Neighborhood"] == neighborhood)]
        group1 = subset[subset["Initial Solution"] == "srz"]["Deviation (%)"]
        group2 = subset[subset["Initial Solution"] == "random"]["Deviation (%)"]
        print(group1)
        print(group2)
        description = f"Comparison of initial solutions (srz vs random) - Pivoting Rule: {pivoting_rule}, Selection Strategy: {neighborhood}"
        perform_wilcoxon_test(group1, group2, "Deviation (%)", description)

print("--" * 50)
# (ii) Which pivoting rule generates better quality solutions and which is faster?
print("Question (ii): Which pivoting rule generates better quality solutions and which is faster?")
pivoting_rules = ["best", "first"]
neighborhoods = ["transpose", "exchange", "insert"]
initial_solutions = ["random", "srz"]
for neighborhood in neighborhoods:
    for initial_solution in initial_solutions:
        subset = data[(data["Neighborhood"] == neighborhood) & (data["Initial Solution"] == initial_solution)]
        group1 = subset[subset["Pivoting Rule"] == "best"]["Deviation (%)"]
        group2 = subset[subset["Pivoting Rule"] == "first"]["Deviation (%)"]
        description = f"Comparison of pivoting rules (best vs first) - Neighborhood: {neighborhood}, Initial Solution: {initial_solution}"
        perform_wilcoxon_test(group1, group2, "Deviation (%)", description)
        group1_time = subset[subset["Pivoting Rule"] == "best"]["Time (s)"]
        group2_time = subset[subset["Pivoting Rule"] == "first"]["Time (s)"]
        description_time = f"Comparison of pivoting rules (best vs first) - Neighborhood: {neighborhood}, Initial Solution: {initial_solution} (Time)"
        perform_wilcoxon_test(group1_time, group2_time, "Time (s)", description_time)

print("--" * 50)
# (iii) Which neighborhood generates better quality solutions and what computation time is required?
print("Question (iii): Which neighborhood generates better quality solutions and what computation time is required?")
pivoting_rules = ["best", "first"]
neighborhoods = ["transpose", "exchange", "insert"]
initial_solutions = ["random", "srz"]

for pivoting_rule in pivoting_rules:
    for initial_solution in initial_solutions:
        subset = data[(data["Pivoting Rule"] == pivoting_rule) & (data["Initial Solution"] == initial_solution)]
        group1 = subset[subset["Neighborhood"] == "transpose"]["Deviation (%)"]
        group2 = subset[subset["Neighborhood"] == "exchange"]["Deviation (%)"]
        description = f"Comparison of neighborhoods (transpose vs exchange) - Pivoting Rule: {pivoting_rule}, Initial Solution: {initial_solution}"
        perform_wilcoxon_test(group1, group2, "Deviation (%)", description)
        group1_time = subset[subset["Neighborhood"] == "transpose"]["Time (s)"]
        group2_time = subset[subset["Neighborhood"] == "exchange"]["Time (s)"]
        description_time = f"Comparison of neighborhoods (transpose vs exchange) - Pivoting Rule: {pivoting_rule}, Initial Solution: {initial_solution} (Time)"
        perform_wilcoxon_test(group1_time, group2_time, "Time (s)", description_time)

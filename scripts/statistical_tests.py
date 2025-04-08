import pandas as pd
from scipy.stats import wilcoxon

# Load the detailed results
data = pd.read_csv("results.csv")

# Function to perform Wilcoxon test and print results
def perform_wilcoxon_test(group1, group2, metric, description):
    stat, p_value = wilcoxon(group1, group2)
    print(f"{description}")
    print(f"Wilcoxon test statistic: {stat}, p-value: {p_value}")
    if p_value < 0.05:
        print("=> Statistically significant difference\n")
    else:
        print("=> No statistically significant difference\n")

# (i) Which initial solution is preferable?
print("Question (i): Which initial solution is preferable?")
subset = data[(data["Neighborhood"] == "insert") & (data["Pivoting Rule"] == "first")]
random_deviation = subset[subset["Initial Solution"] == "random"]["Average Deviation (%)"]
srz_deviation = subset[subset["Initial Solution"] == "srz"]["Average Deviation (%)"]
perform_wilcoxon_test(random_deviation, srz_deviation, "Average Deviation (%)", "Comparison of initial solutions (random vs srz)")

# (ii) Which pivoting rule generates better quality solutions and which is faster?
print("Question (ii): Which pivoting rule generates better quality solutions and which is faster?")
subset = data[(data["Initial Solution"] == "srz") & (data["Neighborhood"] == "insert")]
first_deviation = subset[subset["Pivoting Rule"] == "first"]["Average Deviation (%)"]
best_deviation = subset[subset["Pivoting Rule"] == "best"]["Average Deviation (%)"]
perform_wilcoxon_test(first_deviation, best_deviation, "Average Deviation (%)", "Comparison of pivoting rules (first vs best) - Solution Quality")

first_time = subset[subset["Pivoting Rule"] == "first"]["Total Time (s)"]
best_time = subset[subset["Pivoting Rule"] == "best"]["Total Time (s)"]
perform_wilcoxon_test(first_time, best_time, "Total Time (s)", "Comparison of pivoting rules (first vs best) - Computation Time")

# (iii) Which neighborhood generates better quality solutions and what computation time is required?
print("Question (iii): Which neighborhood generates better quality solutions and what computation time is required?")
subset = data[(data["Initial Solution"] == "srz") & (data["Pivoting Rule"] == "first")]
transpose_deviation = subset[subset["Neighborhood"] == "transpose"]["Average Deviation (%)"]
exchange_deviation = subset[subset["Neighborhood"] == "exchange"]["Average Deviation (%)"]
perform_wilcoxon_test(transpose_deviation, exchange_deviation, "Average Deviation (%)", "Comparison of neighborhoods (transpose vs exchange) - Solution Quality")

transpose_time = subset[subset["Neighborhood"] == "transpose"]["Total Time (s)"]
exchange_time = subset[subset["Neighborhood"] == "exchange"]["Total Time (s)"]
perform_wilcoxon_test(transpose_time, exchange_time, "Total Time (s)", "Comparison of neighborhoods (transpose vs exchange) - Computation Time")
from collections import defaultdict
import csv
from scipy.stats import wilcoxon

# File paths
detailed_results_file = "detailed_results.csv"  # Single neighborhood results
detailed_results_vnd_file = "detailed_results_VND.csv"  # VND results

# Data structures to store results
grouped_results = defaultdict(lambda: {"deviations": [], "times": []})
single_neighborhood_results = defaultdict(lambda: {"deviations": []})
vnd_results = defaultdict(lambda: {"deviations": [], "times": []})

# Read single neighborhood results
with open(detailed_results_file, "r") as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        key = (row["Pivoting Rule"], row["Neighborhood"], row["Instance Size"])
        single_neighborhood_results[key]["deviations"].append(float(row["Deviation (%)"]))

# Read VND results
with open(detailed_results_vnd_file, "r") as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        key = (row["Pivoting Rule"], row["Neighborhood"], row["Initial Solution"], row["Instance Size"])
        vnd_results[key]["deviations"].append(float(row["Deviation (%)"]))
        vnd_results[key]["times"].append(float(row["Time (s)"]))

# Compute statistics and improvement
statistics = {}
for key, data in vnd_results.items():
    avg_deviation = sum(data["deviations"]) / len(data["deviations"]) if data["deviations"] else 0.0
    avg_time = sum(data["times"]) / len(data["times"]) if data["times"] else 0.0

    # Compute improvement over single neighborhoods
    single_neighborhood_devs = [
        sum(single_neighborhood_results[(key[0], nh, key[3])]["deviations"]) /
        len(single_neighborhood_results[(key[0], nh, key[3])]["deviations"])
        for nh in ["exchange", "insert"]
        if (key[0], nh, key[3]) in single_neighborhood_results
    ]
    single_neighborhood_avg_dev = sum(single_neighborhood_devs) / len(single_neighborhood_devs) if single_neighborhood_devs else 0.0
    improvement = ((single_neighborhood_avg_dev - avg_deviation) / single_neighborhood_avg_dev) * 100 if single_neighborhood_avg_dev else 0.0

    statistics[key] = {
        "avg_deviation": avg_deviation,
        "avg_time": avg_time,
        "improvement": improvement
    }

# Save aggregated statistics to a new CSV file
output_file = "aggregated_vnd_statistics.csv"
with open(output_file, "w", newline="") as csvfile:
    fieldnames = ["Pivoting Rule", "Neighborhood", "Initial Solution", "Instance Size", "Avg Deviation (%)", "Avg Time (s)", "Improvement (%)"]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()

    for key, data in statistics.items():
        writer.writerow({
            "Pivoting Rule": key[0],
            "Neighborhood": key[1],
            "Initial Solution": key[2],
            "Instance Size": key[3],
            "Avg Deviation (%)": round(data["avg_deviation"], 2),
            "Avg Time (s)": round(data["avg_time"], 2),
            "Improvement (%)": round(data["improvement"], 2)
        })

print(f"Aggregated statistics saved to {output_file}")


# Compute and print average percentage deviation per instance size
deviation_stats_by_size = defaultdict(list)

for key, data in statistics.items():
    instance_size = key[3]
    deviation_stats_by_size[instance_size].append(data["avg_deviation"])

print("\nAverage Percentage Deviation per Instance Size:")
for size in sorted(deviation_stats_by_size.keys(), key=int):
    deviations = deviation_stats_by_size[size]
    if deviations:

        avg_dev = sum(deviations) / len(deviations)
        print(f"Instance Size {size}: Avg Deviation: {avg_dev:.2f}%")

# Compute and print average computation time per instance size
instance_time_stats = defaultdict(list)

for key, data in statistics.items():
    instance_size = key[3]
    instance_time_stats[instance_size].append(data["avg_time"])

print("\nAverage Computation Time per Instance Size:")
for size in sorted(instance_time_stats.keys(), key=int):
    times = instance_time_stats[size]
    avg_time = sum(times) / len(times) if times else 0.0
    print(f"Instance Size {size}: {avg_time:.2f} seconds")
    
# Compute and print percentage improvement over single neighborhoods per instance size
improvement_stats_by_size = defaultdict(list)

for key, data in statistics.items():
    instance_size = key[3]
    improvement_stats_by_size[instance_size].append(data["improvement"])

print("\nPercentage Improvement over Single Neighborhoods per Instance Size:")
for size in sorted(improvement_stats_by_size.keys(), key=int):
    improvements = improvement_stats_by_size[size]
    if improvements:
        avg_improvement = sum(improvements) / len(improvements)
        print(f"Instance Size {size}: Avg Improvement: {avg_improvement:.2f}%")


print("")

# Perform statistical tests
vnd1_devs = [
    data["avg_deviation"]
    for key, data in statistics.items() if key[1] == "vnd1"
]
vnd2_devs = [
    data["avg_deviation"]
    for key, data in statistics.items() if key[1] == "vnd2"
]

if vnd1_devs and vnd2_devs:
    stat, p_value = wilcoxon(vnd1_devs, vnd2_devs)
    print(f"Wilcoxon test between VND1 and VND2: stat={stat}, p-value={p_value}")
else:
    print("Not enough data for Wilcoxon test.")

# Analyze neighborhood ordering
vnd1_avg_dev = sum(vnd1_devs) / len(vnd1_devs) if vnd1_devs else float('inf')
vnd2_avg_dev = sum(vnd2_devs) / len(vnd2_devs) if vnd2_devs else float('inf')

print(f"VND1 Avg Deviation: {vnd1_avg_dev:.2f}%")
print(f"VND2 Avg Deviation: {vnd2_avg_dev:.2f}%")
print("Preferable ordering:", "VND1" if vnd1_avg_dev < vnd2_avg_dev else "VND2")


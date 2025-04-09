import os
import subprocess
import csv
import time
import signal
import sys
from collections import defaultdict
from multiprocessing import Pool

# Paths
instances_dir = "data/Benchmarks"
best_known_file = "data/bestKnownTCT.txt"
executable = "./Heuristic_PFSP"

# Read best-known values
best_known = {}
with open(best_known_file, "r") as f:
    next(f)  # Skip header
    for line in f:
        instance, value = line.strip().split(",")
        best_known[instance.strip()] = int(value.strip())

# Algorithm parameters
pivoting_rules = ["first", "best"]
neighborhoods = ["vnd1", "vnd2"]
initial_solutions = ["random", "srz"]
num_runs = 10

# Grouped results: (pivoting, neighborhood, init_solution, instance_size) -> [deviations], total_time
grouped_results = defaultdict(lambda: {"deviations": [], "total_time": 0})

# Function to save aggregated results to CSV
def save_results():
    output_file = "results.csv"
    with open(output_file, "w", newline="") as csvfile:
        fieldnames = ["Instance Size", "Pivoting Rule", "Neighborhood", "Initial Solution", "Average Deviation (%)", "Total Time (s)"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()

        for key, data in grouped_results.items():
            pivoting, neighborhood, init_solution, instance_size = key
            avg_dev = sum(data["deviations"]) / len(data["deviations"]) if data["deviations"] else 0.0
            writer.writerow({
                "Instance Size": instance_size,
                "Pivoting Rule": pivoting,
                "Neighborhood": neighborhood,
                "Initial Solution": init_solution,
                "Average Deviation (%)": round(avg_dev, 2),
                "Total Time (s)": round(data["total_time"], 2)
            })
    print(f"\nResults saved to {output_file}")

def save_detailed_results(all_run_results):
    output_file = "detailed_results_VND.csv"
    with open(output_file, "w", newline="") as csvfile:
        fieldnames = ["Instance", "Instance Size", "Pivoting Rule", "Neighborhood", "Initial Solution", "Run", "Deviation (%)", "Time (s)"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        
        for row in all_run_results:
            writer.writerow(row)
    
    print(f"Detailed results saved to {output_file}")


# Signal handler for graceful termination
def signal_handler(sig, frame):
    print("\nTermination signal received. Saving results...")
    save_results()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)



# Function to run a single experiment
def run_experiment(args):
    instance_path, pivoting, neighborhood, init_solution, best_value, num_runs, instance_size = args
    group_key = (pivoting, neighborhood, init_solution, instance_size)
    instance_name = os.path.basename(instance_path)
    run_results = []

    for run in range(num_runs):
        commande = f"{executable} {instance_path} --{pivoting} --{neighborhood} --{init_solution}"
        print(f"Running: {commande}")
        start_time = time.time()
        result = subprocess.run(commande, shell=True, capture_output=True, text=True)
        end_time = time.time()

        try:
            solution_value = int(result.stdout.strip())
        except ValueError:
            print(f"Warning: Unexpected output: {result.stdout.strip()}")
            continue

        deviation = ((solution_value - best_value) / best_value) * 100
        elapsed = end_time - start_time

        run_results.append({
            "Instance": instance_name,
            "Instance Size": instance_size,
            "Pivoting Rule": pivoting,
            "Neighborhood": neighborhood,
            "Initial Solution": init_solution,
            "Run": run + 1,
            "Deviation (%)": round(deviation, 4),
            "Time (s)": round(elapsed, 4),
        })

    return group_key, run_results

# Prepare arguments for multiprocessing
tasks = []
for instance_file in os.listdir(instances_dir):
    instance_path = os.path.join(instances_dir, instance_file)

    # Read instance size from first line
    with open(instance_path, "r") as f:
        first_line = f.readline().strip()
        instance_size = first_line.split()[0]
    
    instance_key = instance_file.upper().replace("TA0", "TA") # fix the name diff between the file and the best known

    
    if instance_key not in best_known:
        continue
    
    best_value = best_known[instance_key]

    for pivoting in pivoting_rules:
        for neighborhood in neighborhoods:
            for init_solution in initial_solutions:
                tasks.append((instance_path, pivoting, neighborhood, init_solution, best_value, num_runs, instance_size))

# Run tasks in parallel
with Pool() as pool:
    results = pool.map(run_experiment, tasks)

# Aggregate results
all_run_results = []

for group_key, run_data in results:
    for row in run_data:
        all_run_results.append(row)

    # Still aggregate if you want the summary file
    deviations = [row["Deviation (%)"] for row in run_data]
    total_time = sum([row["Time (s)"] for row in run_data])
    grouped_results[group_key]["deviations"].extend(deviations)
    grouped_results[group_key]["total_time"] += total_time

# Save both detailed and summary
save_detailed_results(all_run_results)
save_results()

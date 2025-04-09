# Heuristic PFSP Benchmark

This project implements and benchmarks heuristic algorithms for solving the Permutation Flow Shop Problem (PFSP) using C++. It includes various local search techniques, such as iterative improvement and Variable Neighborhood Descent (VND), and provides tools for statistical analysis of the results.

## Project Structure

```
Heuristic_PFSP
├── src
│   ├── main.cpp          # Entry point of the application
│   ├── PFSP.cpp          # Implementation of PFSP algorithms
│   ├── PFSP.h            # Header file for PFSP algorithms
│   ├── utils.cpp         # Utility functions
│   └── utils.h           # Header file for utility functions
├── scripts
│   ├── performance_statistics.py  # Script for running experiments and collecting statistics
│   ├── statistical_tests.py       # Script for performing statistical tests
│   ├── VND_statistics.py          # Script for analyzing VND performance
│   └── test.py                    # Script for testing the implementation
├── data
│   ├── bestKnownTCT.txt  # File containing the best-known TCT values for benchmark instances
│   ├── Benchmarks/       # Directory containing benchmark instances
├── build                 # Directory for compiled object files
├── results.csv           # File to store aggregated results
├── Makefile              # Build instructions
└── README.md             # Project documentation
```

## Building the Project

To build the project, navigate to the root directory and run:

```
make
```

This will compile all the `.cpp` files in the `src` folder and create an executable named `Heuristic_PFSP` in the root directory.

## Running the Project

After building the project, you can run the executable with the following command:

```
./Heuristic_PFSP <arguments>
```

Replace `<arguments>` with the appropriate parameters for your experiment. For example:
- Specify the benchmark instance file.
- Choose the pivoting rule (`first` or `best`).
- Select the neighborhood (`transpose`, `exchange`, `insert`, `vnd1`, `vnd2`).
- Define the initial solution (`random` or `srz`).

for example:
```
./Heuristic_PFSP data/Benchmarks/ta051 --first --transpose --random
```
or 
```
./Heuristic_PFSP data/Benchmarks/ta051 --best --vnd1 --srz
```
to use

Refer to the source code or scripts for detailed argument options.

## Algorithm Details

The project implements heuristic algorithms for solving PFSP, including:
- **Iterative Improvement**: Local search using first-improvement or best-improvement pivoting rules.
- **Variable Neighborhood Descent (VND)**: Combines multiple neighborhoods (`transpose`, `exchange`, `insert`) in a systematic way to escape local optima.
- **Simplified RZ Heuristic**: Used to generate high-quality initial solutions.

The algorithms are benchmarked against standard instances, and their performance is evaluated based on:
- Average percentage deviation from the best-known solutions.
- Computation time required to reach local optima.

## Benchmarking

To benchmark the algorithms, use the provided Python scripts from the root directory:
- **`detailed_single_neighborhood_statistics.py`**: Runs experiments and collects statistics for different algorithm configurations for a single neighborhood. Puts the results in `detailed_results.csv`.
- **`detailed_VND_statistics.py`**: Runs experiments and collects statistics for different algorithm configurations for a VND approach. Puts the results in `detailed_VND_results.csv`.
- **`analyze_single_results`**: Performs statistical tests (e.g., Wilcoxon test) to compare algorithm performance across different configurations of single neighborhoods.
- **`analyze_VND_results`**: Performs statistical tests to compare algorithm performance across different configurations of VND.
The two analysis scripts allow to answere to the assginment questions.
```


## Statistical Analysis

The project includes tools to perform statistical tests to compare:
1. Initial solution methods (`random` vs. `srz`).
2. Pivoting rules (`first` vs. `best`).
3. Neighborhoods (`transpose`, `exchange`, `insert`).

Use the `statistical_tests.py` script or export results to R for advanced analysis.


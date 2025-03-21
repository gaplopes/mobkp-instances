# mobkp-instances

## Overview

This repository contains instances with the respective complete non-dominated set (Pareto front) for the Multi-Objective Binary Knapsack Problem (MOBKP) for different number of objectives and items.

Moreover, it also contains a library for generating and solving instances for the MOBKP for any number of objectives and items.

There are three types of instances available:
- Random instances.
- Negative correlated objectives instances.
- Positive correlated objectives instances.

The random instances are generated with random weights and values for the items.
The correlated instances are generated with correlated objectives, where the correlation can be positive or negative.

This library uses the [mobkp](https://github.com/adbjesus/mobkp) library to solve the MOBKP instances with some of the state-of-the-art algorithms and the procedure described in [Algorithm Selection for Multi-Objective Optimization](https://hal.science/tel-03936989) by Jesus in Section 4.2.1 to generate the correlated instances.

## Features
- Generate random MOBKP instances
- Generate correlated MOBKP instances (positive or negative correlation)
- Save instance data and solutions to files
- Track performance metrics

## Dependencies

This library depends on:

- [mobkp](https://github.com/adbjesus/mobkp): Algorithms to solve the MOBKP instances. (See dependencies of this library in its repository)
- [mooutils](https://github.com/adbjesus/mooutils): Multi-objective optimization utilities, such as quality indicators.
- [CLI11](https://github.com/CLIUtils/CLI11)

## Installation

To generate new instances using this library you can clone the repository and use cmake to build the project.
For that, you can run the following commands:

```bash
git clone
cd mobkp-instances
mkdir build
cd build
cmake ..
cmake --build .
```

## Problem Definition

The MOBKP is a multi-objective optimization problem where the goal is to maximize multiple objectives subject to a set of constraints.
The MOBKP is defined as follows:

```
maximize f(x) = (f_1(x), f_2(x), ..., f_m(x))
subject to
  sum_{j=1}^{n} w_{j} x_{j} <= c
  x_{j} in {0, 1} for j = 1, ..., n
```

where:
- `n` is the number of items.
- `m` is the number of objectives.
- `w_{j}` is the weight of item `j`.
- `c` is the capacity of the knapsack.
- `x_{j}` is a binary variable that indicates if item `j` is selected.
- `f_i(x)` is the value of the `i-th` objective function for the solution `x`.

The MOBKP is a well-known combinatorial optimization problem with applications in several areas, such as finance, engineering, and logistics.

## Usage

The `mobkp-instances` executable allows you to generate new instances for the MOBKP.

The following options are available:

- `--type`: The type of instance to generate. The following types are available:
  - `0`: Random instances.
  - `1`: Negative correlated objectives instances.
  - `2`: Positive correlated objectives instances.

- `--seed`: The seed to use for the random number generator.

- `--n`: The number of items in the instance.

- `--m`: The number of objectives in the instance.

- `--correlation`: The correlation between the objectives. The following values are available:
  - If `--type=1`, then the correlation is the negative correlation factor and the value should be between 0 and -1.
  - If `--type=2`, then the correlation is the positive correlation factor and the value should be between 0 and 1.

- `--timeout`: The maximum time to generate the instance.

- `--weight-factor`: The factor to multiply the total sum of weights of the items.

- `--outfile`: The file to save the instance.

- `--folder-path`: The folder to save the instance.

Example for different types of instances:

```bash
./mobkp-instances --type=0 --n=20 --m=3 --seed=1 --timeout=10 --weight-factor=0.5 # Random instance
./mobkp-instances --type=1 --n=20 --m=3 --seed=1 --correlation=-0.45 --timeout=10 --weight-factor=0.5 # Negative correlated instance
./mobkp-instances --type=2 --n=20 --m=3 --seed=1 --correlation=0.45 --timeout=10 --weight-factor=0.5 # Positive correlated instance
```

## Instances

The instances are stored in the `instances/` directory. A more detailed description of the instances is provided in the `instances/README.md` file.

## Library Functions

The library provides the following parameters and functions to generate instances:

### Parameter Handling

```cpp
#include <parameters.hpp>

// Create parameters for instance generation
mobkp_instances::Parameters params(
    int32_t type,           // 0: random, 1: negative correlation, 2: positive correlation
    int32_t n,              // Number of items
    int32_t m,              // Number of objectives
    int64_t seed,           // Random seed
    double correlation,     // Correlation coefficient (-1 to 1)
    double weight_factor,   // Weight capacity factor (0 to 1)
    double timeout,         // Maximum solution time in seconds
    std::string folder_path, // Output folder path
    std::string outfile     // Output file name
);
```

### Instance Generation

```cpp
// Generate a random MOBKP instance
mobkp_instances::random(params);

// Generate a correlated MOBKP instance (positive or negative)
mobkp_instances::correlated(params);
```

## Scripts

The `scripts/` directory contains example scripts to generate instances.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## References

If you use these library or instances, please cite in your work using the citation available in the repository.

## Contributing

Contributions are welcome! If you have any instances or improvements to add, please fork the repository and create a pull request.

## Contact

For any questions or issues, please open an issue on the repository or contact the author at galopes@dei.uc.pt or via GitHub or LinkedIn (see profile for contact information).

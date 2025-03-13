#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <parameters.hpp>
#include <solver.hpp>

int main(int argc, char* argv[]) {
  try {
    CLI::App app{"Instances for the Multi-Objective Binary Knapsack Problem (MOBKP)"};

    int32_t type, n, m;
    int64_t seed;
    std::string outfile = "", folder_path = "";
    double correlation, timeout, weight_factor;

    app.add_option("--type", type, "Type of instance to generate (0: random, 1: negative correlation, 2: positive correlation)")
        ->required()
        ->check(CLI::Range(0, 2));
    app.add_option("--n", n, "Number of variables")
        ->required()
        ->check(CLI::PositiveNumber);
    app.add_option("--m", m, "Number of objectives")
        ->required()
        ->check(CLI::PositiveNumber);
    app.add_option("--seed", seed, "Seed value")
        ->default_val(time(0))
        ->check(CLI::PositiveNumber);
    app.add_option("--correlation", correlation, "Correlation value between objectives")
        ->default_val(0.0)
        ->check(CLI::Range(-1.0, 1.0));
    app.add_option("--timeout", timeout, "Timeout value in seconds")
        ->default_val(86400.0 * 30)
        ->check(CLI::PositiveNumber);
    app.add_option("--weight-factor", weight_factor, "Weight factor")
        ->default_val(0.5)
        ->check(CLI::Range(0.0, 1.0));
    app.add_option("--folder-path", folder_path, "Folder path to save the instances");
    app.add_option("--outfile", outfile, "Output file name");

    // Parse the command line arguments
    CLI11_PARSE(app, argc, argv);

    // Create the parameters object
    mobkp_instances::Parameters params(type, n, m, seed, correlation, weight_factor, timeout, folder_path, outfile);

    // Generate the instance
    switch (type) {
      case 0:
        mobkp_instances::random(params);
        break;
      case 1:
      case 2:
        mobkp_instances::correlated(params);
        break;
      default:
        throw std::invalid_argument("Invalid instance type");
    }
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

// Examples: 
// - Random instance: ./mobkp-instances --type=0 --n=20 --m=3 --seed=1 --timeout=10 --weight-factor=0.5 --folder-path=../instances --outfile=20_1.in
// - Negative correlation instance: ./mobkp-instances --type=1 --n=20 --m=3 --seed=1 --correlation=-0.5 --timeout=10 --weight-factor=0.5 --folder-path=../instances --outfile=20_1_-0.5.in
// - Positive correlation instance: ./mobkp-instances --type=2 --n=20 --m=3 --seed=1 --correlation=0.5 --timeout=10 --weight-factor=0.5 --folder-path=../instances --outfile=20_1_0.5.in

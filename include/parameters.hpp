#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

/**
 * @file parameters.hpp
 * @brief Parameter handling for MOBKP instance generation
 *
 * This header defines the Parameters struct used to configure MOBKP instance
 * generation, including validation of parameter ranges and automatic path setup.
 */

#include <fmt/format.h>  // For string formatting

#include <filesystem>  // For directory and file path handling
#include <string>      // For std::string
#include <vector>      // For std::vector

namespace mobkp_instances {

/**
 * @brief Parameters for MOBKP instance generation
 *
 * This struct contains all the parameters required for generating
 * Multi-Objective Binary Knapsack Problem (MOBKP) instances.
 */
struct Parameters {
  int32_t type;
  int32_t n;
  int32_t m;
  int64_t seed;
  double correlation;
  double weight_factor;
  double timeout;
  std::string folder_path;
  std::string outfile;
  std::string r_script_path;

  /**
   * @brief Constructs a Parameters object with the given values
   *
   * @param type Instance type: 0 (random), 1 (negative correlation), 2 (positive correlation)
   * @param n Number of variables (items) in the knapsack problem
   * @param m Number of objectives in the multi-objective problem
   * @param seed Random seed for reproducibility
   * @param correlation Correlation between objectives (-1 to 1)
   * @param weight_factor Factor to determine knapsack capacity (0 to 1)
   * @param timeout Maximum solution time in seconds
   * @param folder_path Path to save the instance files (optional)
   * @param outfile Output file name (optional)
   * @param r_script_path Path to the R generator script (optional, uses default if empty)
   */
  Parameters(int32_t type, int32_t n, int32_t m, int64_t seed,
             double correlation, double weight_factor, double timeout,
             std::string folder_path, std::string outfile,
             std::string r_script_path = "")
      : type(type), n(n), m(m), seed(seed), correlation(correlation),
        weight_factor(weight_factor), timeout(timeout),
        folder_path(folder_path), outfile(outfile), r_script_path(r_script_path) {
    validate();     // Validate the parameters
    setup_paths();  // Set up paths if not provided
  }

  /**
   * @brief Validates the parameters to ensure they're within acceptable ranges
   *
   * @throws std::invalid_argument If any parameter is outside its valid range
   */
  void validate() const {
    // Check number of objectives
    if (m <= 1) {
      throw std::invalid_argument("Number of objectives must be greater than 1");
    }

    // Check number of variables
    if (n <= 0) {
      throw std::invalid_argument("Number of variables must be positive");
    }

    // Check seed
    if (seed < 0) {
      throw std::invalid_argument("Seed must be non-negative");
    }

    // Check weight factor
    if (weight_factor < 0.0 || weight_factor > 1.0) {
      throw std::invalid_argument("Weight factor must be between 0 and 1");
    }

    // Check timeout
    if (timeout <= 0.0) {
      throw std::invalid_argument("Timeout must be positive");
    }

    // Check correlation based on instance type
    if (type == 1 && (correlation >= 0.0 || correlation < -1.0 / (m - 1))) {
      throw std::invalid_argument(
          fmt::format("For negative correlation, value must be between -{:.4f} and 0.0",
                      1.0 / (m - 1)));
    } else if (type == 2 && (correlation <= 0.0 || correlation > 1.0)) {
      throw std::invalid_argument(
          fmt::format("For positive correlation, value must be between 0.0 and 1"));
    }
  }

  /**
   * @brief Sets up the folder path and output filename if not provided
   *
   * Creates the necessary directories for the output files.
   */
  void setup_paths() {
    // Set up folder path if not provided
    if (folder_path.empty()) {
      // Folders for each type of instance
      static const std::vector<std::string> type_folders = {"random", "negative", "positive"};

      // Create path based on type and dimension
      folder_path = "../instances/" +
                    type_folders[static_cast<int>(type)] +
                    "/" + std::to_string(m) + "D/";
    } else {
      // Add trailing slash if not present
      if (folder_path.back() != '/') {
        folder_path += "/";
      }
    }

    // Create directories if they don't exist
    std::filesystem::create_directories(folder_path);

    // Set up output filename if not provided
    if (outfile.empty()) {
      outfile = fmt::format("{}_{}{}.in",
                            n, seed,
                            type == 0 ? "" : fmt::format("_{:.4f}", correlation));
    }
  }

  /**
   * @brief Converts the parameters to a human-readable string
   *
   * @return A formatted string representation of the parameters
   */
  std::string to_string() const {
    return fmt::format(
        "Instance Parameters:\n"
        "  Type: {}\n"
        "  Size: {} items, {} objectives\n"
        "  Seed: {}\n"
        "  Correlation: {:.4f}\n"
        "  Weight Factor: {:.2f}\n"
        "  Timeout: {:.1f}s\n"
        "  Output: {}/{}",
        type, n, m, seed, correlation,
        weight_factor, timeout,
        folder_path, outfile);
  }
};

}  // namespace mobkp_instances

#endif  // PARAMETERS_HPP

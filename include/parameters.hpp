#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <fmt/format.h>

#include <filesystem>
#include <string>
#include <vector>

namespace mobkp_instances {

struct Parameters {
  int32_t type;             // Type of instance: 0 (random), 1 (negative correlation), 2 (positive correlation)
  int32_t n;                // Number of variables
  int32_t m;                // Number of objectives
  int64_t seed;             // Random seed
  double correlation;       // Correlation between objectives
  double weight_factor;     // Weight factor
  double timeout;           // Timeout in seconds
  std::string folder_path;  // Folder path to save the instances
  std::string outfile;      // Output file name

  // Constructor
  Parameters(int32_t type, int32_t n, int32_t m, int64_t seed,
             double correlation, double weight_factor, double timeout,
             std::string folder_path, std::string outfile)
      : type(type), n(n), m(m), seed(seed), correlation(correlation), weight_factor(weight_factor), timeout(timeout), folder_path(folder_path), outfile(outfile) {
    validate();
    setup_paths();
  }

  void validate() const {
    if (m <= 1) {
      throw std::invalid_argument("Number of objectives must be greater than 1");
    }
    if (n <= 0) {
      throw std::invalid_argument("Number of variables must be positive");
    }
    if (seed < 0) {
      throw std::invalid_argument("Seed must be non-negative");
    }
    if (weight_factor < 0.0 || weight_factor > 1.0) {
      throw std::invalid_argument("Weight factor must be between 0 and 1");
    }
    if (timeout <= 0.0) {
      throw std::invalid_argument("Timeout must be positive");
    }
    if (type == 1 && (correlation >= 0.0 || correlation < -1.0 / (m - 1))) {
      throw std::invalid_argument(fmt::format("For negative correlation, value must be between -{:.4f} and 0.0", 1.0 / (m - 1)));
    } else if (type == 2 && (correlation <= 0.0 || correlation > 1.0 / (m - 1))) {
      throw std::invalid_argument(fmt::format("For positive correlation, value must be between 0.0 and {:.4f}", 1.0 / (m - 1)));
    }
  }

  void setup_paths() {
    if (folder_path.empty()) {
      static const std::vector<std::string> type_folders = {"random", "neg_corr", "pos_corr"};
      folder_path = "../instances/" + type_folders[static_cast<int>(type)] + "/" + std::to_string(m) + "D";
    }
    std::filesystem::create_directories(folder_path);

    if (outfile.empty()) {
      outfile = fmt::format("{}_{}{}.in", n, seed, type == 0 ? "" : fmt::format("_{:.4f}", correlation));
    }
  }

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

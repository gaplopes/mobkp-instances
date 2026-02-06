#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <parameters.hpp>
#include <solver.hpp>

/**
 * @brief Parse a range string like "20-100:10" into a vector of values
 * @param range_str String in format "start-end:step" or "start-end" (step=1)
 * @return Vector of integer values in the range
 */
std::vector<int32_t> parse_range(const std::string &range_str) {
  std::vector<int32_t> values;
  
  // Check if it's a range (contains '-')
  size_t dash_pos = range_str.find('-');
  if (dash_pos == std::string::npos) {
    // Single value
    values.push_back(std::stoi(range_str));
    return values;
  }
  
  int32_t start = std::stoi(range_str.substr(0, dash_pos));
  
  size_t colon_pos = range_str.find(':');
  int32_t end, step;
  
  if (colon_pos != std::string::npos) {
    end = std::stoi(range_str.substr(dash_pos + 1, colon_pos - dash_pos - 1));
    step = std::stoi(range_str.substr(colon_pos + 1));
  } else {
    end = std::stoi(range_str.substr(dash_pos + 1));
    step = 1;
  }
  
  for (int32_t v = start; v <= end; v += step) {
    values.push_back(v);
  }
  
  return values;
}

/**
 * @brief Parse a comma-separated list of doubles
 * @param list_str String in format "val1,val2,val3"
 * @return Vector of double values
 */
std::vector<double> parse_double_list(const std::string &list_str) {
  std::vector<double> values;
  std::stringstream ss(list_str);
  std::string item;
  
  while (std::getline(ss, item, ',')) {
    values.push_back(std::stod(item));
  }
  
  return values;
}

/**
 * @brief Validate correlation value based on type and number of objectives
 * @param type Instance type (1: negative, 2: positive)
 * @param m Number of objectives
 * @param correlation Correlation value to validate
 * @throws std::invalid_argument if correlation is out of valid range
 */
void validate_correlation(int32_t type, int32_t m, double correlation) {
  double max_abs_correlation = 1.0 / (m - 1);
  
  if (type == 1) {  // Negative correlation
    if (correlation >= 0.0 || correlation < -max_abs_correlation) {
      throw std::invalid_argument(
          fmt::format("For negative correlation with m={}, correlation must be in range ({:.4f}, 0). Got: {:.4f}",
                      m, -max_abs_correlation, correlation));
    }
  } else if (type == 2) {  // Positive correlation
    if (correlation <= 0.0 || correlation > max_abs_correlation) {
      throw std::invalid_argument(
          fmt::format("For positive correlation with m={}, correlation must be in range (0, {:.4f}]. Got: {:.4f}",
                      m, max_abs_correlation, correlation));
    }
  }
}

int main(int argc, char* argv[]) {
  try {
    CLI::App app{"Instances for the Multi-Objective Binary Knapsack Problem (MOBKP)"};

    int32_t type, n, m;
    int64_t seed;
    std::string outfile = "", folder_path = "", r_script_path = "";
    std::string n_range = "", seed_range = "", correlation_list = "";
    double correlation, timeout, weight_factor;

    app.add_option("--type", type, "Type of instance to generate (0: random, 1: negative correlation, 2: positive correlation)")
        ->required()
        ->check(CLI::Range(0, 2));
    app.add_option("--m", m, "Number of objectives")
        ->required()
        ->check(CLI::PositiveNumber);
    
    // Single value options
    app.add_option("--n", n, "Number of variables (single instance mode)")
        ->check(CLI::PositiveNumber);
    app.add_option("--seed", seed, "Seed value (single instance mode)")
        ->default_val(time(0))
        ->check(CLI::PositiveNumber);
    app.add_option("--correlation", correlation, "Correlation value between objectives (single instance mode)")
        ->default_val(0.0)
        ->check(CLI::Range(-1.0, 1.0));
    
    // Batch processing options
    app.add_option("--n-range", n_range, 
        "Range of n values in format 'start-end:step' or 'start-end' (batch mode). Example: 20-100:10");
    app.add_option("--seed-range", seed_range, 
        "Range of seed values in format 'start-end' (batch mode). Example: 1-10");
    app.add_option("--correlation-list", correlation_list, 
        "Comma-separated correlation values (batch mode). Example: -0.45,-0.25,-0.1\n"
        "NOTE: For correlated instances, |correlation| must be < 1/(m-1)");
    
    // Common options
    app.add_option("--timeout", timeout, "Timeout value in seconds")
        ->default_val(86400.0 * 30)
        ->check(CLI::PositiveNumber);
    app.add_option("--weight-factor", weight_factor, "Weight factor")
        ->default_val(0.5)
        ->check(CLI::Range(0.0, 1.0));
    app.add_option("--folder-path", folder_path, "Folder path to save the instances");
    app.add_option("--outfile", outfile, "Output file name (only for single instance mode)");
    app.add_option("--r-script", r_script_path, "Path to R generator script (default: ../include/generator.R)");

    // Parse the command line arguments
    CLI11_PARSE(app, argc, argv);

    // Determine batch mode vs single mode
    bool batch_mode = !n_range.empty() || !seed_range.empty() || !correlation_list.empty();
    
    if (batch_mode) {
      // Batch processing mode
      std::vector<int32_t> n_values = n_range.empty() ? std::vector<int32_t>{n} : parse_range(n_range);
      std::vector<int32_t> seed_values = seed_range.empty() ? std::vector<int32_t>{static_cast<int32_t>(seed)} : parse_range(seed_range);
      std::vector<double> corr_values = correlation_list.empty() ? std::vector<double>{correlation} : parse_double_list(correlation_list);
      
      // Validate all correlation values upfront for correlated instances
      if (type != 0) {
        for (double c : corr_values) {
          validate_correlation(type, m, c);
        }
      }
      
      int total = n_values.size() * seed_values.size() * (type == 0 ? 1 : corr_values.size());
      int count = 0;
      
      std::cout << "Batch mode: generating " << total << " instances" << std::endl;
      std::cout << "  n values: " << n_values.size() << ", seed values: " << seed_values.size();
      if (type != 0) {
        std::cout << ", correlation values: " << corr_values.size();
      }
      std::cout << std::endl << std::endl;
      
      for (int32_t n_val : n_values) {
        for (double c_val : (type == 0 ? std::vector<double>{0.0} : corr_values)) {
          for (int32_t s_val : seed_values) {
            count++;
            std::cout << "[" << count << "/" << total << "] n=" << n_val << ", seed=" << s_val;
            if (type != 0) std::cout << ", correlation=" << c_val;
            std::cout << std::endl;
            
            mobkp_instances::Parameters params(type, n_val, m, s_val, c_val, weight_factor, 
                                                timeout, folder_path, "", r_script_path);
            
            switch (type) {
              case 0:
                mobkp_instances::random(params);
                break;
              case 1:
              case 2:
                mobkp_instances::correlated(params);
                break;
            }
          }
        }
      }
      
      std::cout << std::endl << "Batch processing complete: " << count << " instances generated." << std::endl;
      
    } else {
      // Single instance mode (original behavior)
      if (n == 0) {
        throw std::invalid_argument("Either --n or --n-range must be specified");
      }
      
      // Validate correlation for correlated instances
      if (type != 0) {
        validate_correlation(type, m, correlation);
      }
      
      mobkp_instances::Parameters params(type, n, m, seed, correlation, weight_factor, timeout, folder_path, outfile, r_script_path);

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
    }
    
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}

// Examples: 
// Single instance mode:
// - Random instance: ./mobkp-instances --type=0 --n=20 --m=3 --seed=1 --timeout=10 --weight-factor=0.5
// - Negative correlation: ./mobkp-instances --type=1 --n=20 --m=3 --seed=1 --correlation=-0.3 --timeout=10
// - Positive correlation: ./mobkp-instances --type=2 --n=20 --m=3 --seed=1 --correlation=0.3 --timeout=10
//
// Batch mode:
// - Random instances: ./mobkp-instances --type=0 --m=2 --n-range=50-200:50 --seed-range=1-5
// - Negative with multiple correlations: ./mobkp-instances --type=1 --m=3 --n-range=20-50:10 --seed-range=1-3 --correlation-list=-0.45,-0.25,-0.1
// - Positive with multiple correlations: ./mobkp-instances --type=2 --m=3 --n-range=20-50:10 --seed-range=1-3 --correlation-list=0.45,0.25,0.1

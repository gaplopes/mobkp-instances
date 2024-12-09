#ifndef PARSER_HPP
#define PARSER_HPP

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

class Arguments {
 public:
  Arguments(int argc, char **argv) : argc(argc), argv(argv) {
    type = 0;
    seed = time(0);
    correlation = 0.0;
    n = 0;
    m = 0;
    weight_factor = 0.5;
    timeout = 604800.0;
    parse_arguments(argv);
    validate_arguments();
  }

  static void print_usage() {
    std::cout << "Usage: [options]\n"
              << "--type=<0|1|2>          Type of instance (0: random, 1: negative correlation, 2: positive correlation)\n"
              << "--outfile=<filename>    Output file name\n"
              << "--seed=<number>         Seed value\n"
              << "--correlation=<number>  Correlation value between objectives: -1.0 <= correlation < 0.0 (negative), 0.0 < correlation <= 1.0 (positive)\n"
              << "--n=<number>            Value of n (number of variables)\n"
              << "--m=<number>            Value of m (number of objectives)\n"
              << "--weight-factor=<number> Weight factor\n"
              << "--timeout=<number>      Timeout value in seconds\n"
              << "Default values: type=0, outfile=n_seed.in, seed=time(0), correlation=0.0, weight-factor=0.5, timeout=7 days\n";
  }

  int32_t get_type() const { return type; }
  std::string get_outfile() const { return outfile; }
  int64_t get_seed() const { return seed; }
  double get_correlation() const { return correlation; }
  int32_t get_n() const { return n; }
  int32_t get_m() const { return m; }
  double get_timeout() const { return timeout; }
  double get_weight_factor() const { return weight_factor; }
  std::string get_folder_path() const { return folder_path; }

  void print_arguments() {
    std::cout << "type: " << type << std::endl;
    std::cout << "seed: " << seed << std::endl;
    std::cout << "correlation: " << correlation << std::endl;
    std::cout << "n: " << n << std::endl;
    std::cout << "m: " << m << std::endl;
    std::cout << "timeout: " << timeout << std::endl;
    std::cout << "weight_factor: " << weight_factor << std::endl;
    std::cout << "folder_path: " << folder_path << std::endl;
    std::cout << "outfile: " << outfile << std::endl;
  }

 private:
  int argc;
  char **argv;
  int32_t type;
  std::string outfile;
  int64_t seed;
  double correlation;
  int32_t n;
  int32_t m;
  double timeout;
  double weight_factor;
  std::string folder_path;

  void parse_arguments(char **argv) {
    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];
      size_t pos = arg.find('=');
      std::string key = arg.substr(0, pos);
      std::string value = (pos != std::string::npos) ? arg.substr(pos + 1) : "";

      if (key == "--type") {
        type = std::stoi(value);
      } else if (key == "--outfile") {
        outfile = value;
      } else if (key == "--seed") {
        seed = std::stoi(value);
      } else if (key == "--correlation") {
        correlation = std::stod(value);
      } else if (key == "--n") {
        n = std::stoi(value);
      } else if (key == "--m") {
        m = std::stoi(value);
      } else if (key == "--weight-factor") {
        weight_factor = std::stod(value);
      } else if (key == "--timeout") {
        timeout = std::stod(value);
      } else {
        print_usage();
        throw std::invalid_argument("Unknown argument: " + key);
      }
    }
  }

  void validate_arguments() {
    if (type < 0 || type > 2) {
      throw std::invalid_argument("Invalid type value. Must be between 0 and 2.");
    }
    if (seed < 0) {
      throw std::invalid_argument("Seed must be non-negative.");
    }
    if (type != 0) {
      if (type == 1) {
        if (correlation >= 0.0 || correlation <= -1.0 / (m - 1)) {
          throw std::invalid_argument("Correlation must be between -1/(m-1) and 0.0.");
        }
      } else {
        if (correlation <= 0.0 || correlation >= 1.0 / (m - 1)) {
          throw std::invalid_argument("Correlation must be between 0.0 and 1/(m-1).");
        }
      }
    }
    if (n <= 0) {
      throw std::invalid_argument("n must be greater than 0.");
    }
    if (m <= 1) {
      throw std::invalid_argument("m must be greater than 1.");
    }
    if (timeout <= 0.0) {
      throw std::invalid_argument("Timeout must be greater than 0.0.");
    }
    if (weight_factor < 0.0 || weight_factor > 1.0) {
      throw std::invalid_argument("Weight factor must be between 0.0 and 1.0.");
    }
    folder_path = create_folder_path();
    if (folder_path.empty()) {
      throw std::runtime_error("Folder path is empty.");
    }
    if (outfile.empty()) {
      outfile = create_outfile();
    }
    if (outfile.empty()) {
      throw std::runtime_error("Outfile is empty.");
    }
  }

  std::string create_folder_path() {
    static const std::string folder_types[] = {"random/", "neg_corr/", "pos_corr/"};
    std::string path = "../instances/";
    if (type >= 0 && type < 3) {
      path += folder_types[type];
    }
    path += std::to_string(m) + "D/";
    return path;
  }

  std::string create_outfile() {
    return std::to_string(n) + "_" + std::to_string(seed) +
           (type == 0 ? ".in" : "_" + std::to_string(correlation) + ".in");
  }
};

#endif  // PARSER_HPP

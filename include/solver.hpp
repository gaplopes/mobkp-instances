#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <cassert>
#include <filelock.hpp>
#include <filesystem>
#include <fstream>
#include <mobkp/anytime_trace.hpp>
#include <mobkp/dp.hpp>
#include <mobkp/problem.hpp>
#include <mobkp/scalarization.hpp>
#include <mobkp/solution.hpp>
#include <mooutils/indicators.hpp>
#include <random>
#include <vector>

// Type definitions for MOBKP solver
using data_type = int_fast64_t;
using dvec_type = std::vector<bool>;
using ovec_type = std::vector<data_type>;
using cvec_type = std::vector<data_type>;
using hv_data_type = boost::multiprecision::int256_t;
using problem_type = mobkp::ordered_problem<mobkp::problem<data_type>>;
using solution_type = mobkp::solution<problem_type, dvec_type, ovec_type, cvec_type>;

namespace mobkp_instances {

namespace internal {
/**
 * @brief Saves instance statistics to a CSV file
 *
 * @param folder_path Path to store the statistics file
 * @param n Number of variables
 * @param m Number of objectives
 * @param rho Correlation coefficient
 * @param seed Random seed
 * @param time Solution time in seconds
 * @param n_solutions Number of non-dominated solutions
 */
void save_stats_to_file(const std::string &folder_path,
                        int32_t n, int32_t m, double rho, int64_t seed,
                        double time, int32_t n_solutions) {
  if (!std::filesystem::exists(folder_path)) {
    std::filesystem::create_directory(folder_path);
  }
  const std::string file_name = fmt::format("times{}D.csv", m);
  const std::string file_path = folder_path + file_name;

  // Open the file in append mode
  std::ofstream file_stream(file_path, std::ios::app);
  if (!file_stream.is_open()) {
    std::cerr << "Failed to open file: " << file_path << " - " << strerror(errno) << std::endl;
    return;
  }

  FILE *file_ptr = fopen(file_path.c_str(), "a");
  if (!file_ptr) {
    throw std::runtime_error("Failed to get file descriptor");
  }

  int fd = fileno(file_ptr);
  if (fd == -1) {
    throw std::runtime_error("Failed to get file descriptor number");
  }

  {  // RAII scope for file lock
    FileLock lock(fd);
    fmt::print(file_stream, "{},{},{},{:.4f},{:.4f},{}\n",
               m, n, seed, rho, time, n_solutions);
  }
}

/**
 * @brief Writes solution data to output file
 *
 * @param folder_path Path to store the solution
 * @param file_name Output file name
 * @param problem The MOBKP problem instance
 * @param solutions Set of non-dominated solutions
 */
void write_solution(const std::string &folder_path, const std::string &file_name,
                    const mobkp::problem<data_type> &problem,
                    const mooutils::unordered_set<solution_type> &solutions) {
  if (!std::filesystem::exists(folder_path)) {
    std::filesystem::create_directory(folder_path);
  }
  const std::string file_path = folder_path + file_name;
  std::cout << "Saving solution to: " << file_path << std::endl;
  auto solution_stream = std::ofstream(file_path);
  fmt::print(solution_stream, "{} {}\n", problem.num_items(), problem.num_objectives());
  fmt::print(solution_stream, "{}\n", problem.weight_capacity(0));
  for (size_t i = 0; i < problem.num_items(); ++i) {
    fmt::print(solution_stream, "{} {:d}\n", problem.item_weights(i).back(), fmt::join(problem.item_values(i), " "));
  }
  fmt::print(solution_stream, "{}\n", solutions.size());
  for (auto const &s : solutions) {
    fmt::print(solution_stream, "{:d}\n", fmt::join(s.objective_vector(), " "));
  }
  solution_stream.close();
}

/**
 * @brief Solves a MOBKP instance using Dynamic Programming
 *
 * @param timeout Maximum solution time in seconds
 * @param n Number of variables
 * @param m Number of objectives
 * @param points Vector with problem data
 * @return Pair of problem instance and non-dominated solutions
 */
auto solve_mobkp(const double timeout, const int32_t n, const int32_t m, std::vector<data_type> points) {
  const auto orig_problem = mobkp::problem<data_type>(n, m, 1, std::move(points));

  std::vector<size_t> index_order(n);
  std::iota(index_order.begin(), index_order.end(), 0);
  const auto problem = problem_type(orig_problem, index_order);

  auto solutions = mooutils::unordered_set<solution_type>();
  auto hvref = ovec_type(m, -1);
  auto anytime_trace = mobkp::anytime_trace(mooutils::incremental_hv<hv_data_type, ovec_type>(hvref));
  switch (m) {
    case 2:
      solutions = mobkp::fpsv_dp<solution_type>(problem, anytime_trace, timeout);
      break;
    default:
      // solutions = mobkp::nemull_dp<solution_type>(problem, anytime_trace, timeout);
      solutions = mobkp::bhv_dp<solution_type>(problem, anytime_trace, timeout);
      break;
  }
  return std::make_pair(orig_problem, solutions);
}

/**
 * @brief Generates random MOBKP instance and solves it
 */
void random_mobkp(const int32_t n, const int32_t m, const int64_t seed,
                  const double weight_factor, const double timeout,
                  const std::string folder_path, const std::string outfile,
                  const int64_t MAX = 300) {
  // Validate the input parameters
  assert(m > 1);
  assert(n > 0);
  assert(seed >= 0);
  assert(rho >= -1.0 && rho <= 1.0);
  assert(weight_factor >= 0.0 && weight_factor <= 1.0);
  assert(timeout > 0.0);
  // Seed the random number generator
  std::srand(seed);
  // Generate the instance values
  std::vector<int64_t> points(n * (m + 1));
  int64_t total_weight = 0;
  for (int i = 0; i < n; i++) {
    int64_t weight = (std::rand() % (MAX - 1)) + 1;
    for (int j = 0; j < m; j++) {
      int64_t num = (std::rand() % (MAX - 1)) + 1;
      int32_t index = (i * (m + 1)) + j;
      points[index] = num;
    }
    points[(i * (m + 1)) + m] = weight;
    total_weight += weight;
  }
  int64_t W = std::round(total_weight * weight_factor);
  points.insert(points.begin(), W);
  // Solve the instance
  auto start_time = std::chrono::high_resolution_clock::now();
  auto problem_solutions = solve_mobkp(timeout, n, m, points);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
  auto problem = problem_solutions.first;
  auto solutions = problem_solutions.second;
  // Save the stats and solutions
  save_stats_to_file(folder_path, n, m, 0, seed, duration.count(), solutions.size());
  write_solution(folder_path, outfile, problem, solutions);
}

/**
 * @brief Generates correlated MOBKP instance and solves it
 */
void correlated_mobkp(const int32_t n, const int32_t m, const double rho, const int64_t seed,
                      const double weight_factor, const double timeout,
                      const std::string folder_path, const std::string outfile) {
  // Validate the input parameters
  assert(m > 1);
  assert(n > 0);
  assert(seed >= 0);
  assert(rho >= -1.0 && rho <= 1.0);
  assert(weight_factor >= 0.0 && weight_factor <= 1.0);
  assert(timeout > 0.0);
  // Set the file path
  std::string file_path = folder_path + outfile;
  // Generate the instance using R script
  const std::string r_script_path = "../include/generator.R";
  const std::string rho_str = fmt::format("{:.2f}", rho);
  const std::string command = fmt::format("./{} {} {} {} {} {} {} {}", r_script_path, n, m, rho, 0, weight_factor, seed, file_path);
  int result = system(command.c_str());
  if (result != 0) {
    throw std::runtime_error("Command execution failed with status: " + std::to_string(result));
  }
  // Read the instance from the file
  auto fin = std::fstream(file_path, std::ios::in);
  if (!fin.is_open()) {
    fmt::print("Error: Could not open file {}\n", file_path);
    exit(1);
  }
  int32_t _n, _m;
  fin >> _n >> _m;
  int64_t W;
  fin >> W;
  std::vector<int64_t> points(_n * (_m + 1));
  for (int i = 0; i < _n; i++) {
    for (int j = 0; j < _m; j++) {
      int64_t num;
      fin >> num;
      int32_t index = (i * (_m + 1)) + j;
      points[index] = num;
    }
    int64_t weight;
    fin >> weight;
    points[(i * (_m + 1)) + _m] = weight;
  }
  points.insert(points.begin(), W);
  // Solve the instance
  auto start_time = std::chrono::high_resolution_clock::now();
  auto problem_solutions = solve_mobkp(timeout, n, m, points);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
  auto problem = problem_solutions.first;
  auto solutions = problem_solutions.second;
  // Save the stats and solutions
  save_stats_to_file(folder_path, n, m, rho, seed, duration.count(), solutions.size());
  write_solution(folder_path, outfile, problem, solutions);
}

}  // namespace internal

/**
 * @brief Interface to generate random MOBKP instance
 * 
 * @param params Parameters object with instance settings
 */
void random(const Parameters &params) {
  internal::random_mobkp(params.n, params.m, params.seed,
                         params.weight_factor, params.timeout,
                         params.folder_path,
                         params.outfile);
}

/**
 * @brief Interface to generate correlated MOBKP instance
 * 
 * @param params Parameters object with instance settings
 */
void correlated(const Parameters &params) {
  internal::correlated_mobkp(params.n, params.m, params.correlation, params.seed,
                             params.weight_factor, params.timeout,
                             params.folder_path,
                             params.outfile);
}

}  // namespace mobkp_instances

#endif  // SOLVER_HPP

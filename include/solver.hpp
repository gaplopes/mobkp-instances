#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/multiprecision/cpp_int.hpp>
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

using data_type = int_fast64_t;
using dvec_type = std::vector<bool>;
using ovec_type = std::vector<data_type>;
using cvec_type = std::vector<data_type>;
using hv_data_type = boost::multiprecision::int256_t;
using problem_type = mobkp::ordered_problem<mobkp::problem<data_type>>;
using solution_type = mobkp::solution<problem_type, dvec_type, ovec_type, cvec_type>;

void write_solution(const std::string &folder_path, const std::string &file_name,
                    const mobkp::problem<data_type> &problem, const mooutils::unordered_set<solution_type> &solutions) {
  if (!std::filesystem::exists(folder_path)) {
    std::filesystem::create_directory(folder_path);
  }
  const std::string file_path = folder_path + "/" + file_name;
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

auto solve_mobkp(const Arguments &args, std::vector<data_type> points) {
  const double timeout = args.get_timeout();
  const int32_t n = args.get_n();
  const int32_t m = args.get_m();

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
      solutions = mobkp::nemull_dp<solution_type>(problem, anytime_trace, timeout);
      break;
  }
  return std::make_pair(orig_problem, solutions);
}

void generate_random_mobkp_test(const Arguments &args, const int32_t MAX = 300) {
  const int32_t n = args.get_n();
  const int32_t m = args.get_m();
  std::srand(args.get_seed());

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
  int64_t W = std::round(total_weight * args.get_weight_factor());
  points.insert(points.begin(), W);

  auto problem_solutions = solve_mobkp(args, points);
  auto problem = problem_solutions.first;
  auto solutions = problem_solutions.second;

  write_solution(args.get_folder_path(), args.get_outfile(), problem, solutions);
}

void generate_corr_mobkp_test(const Arguments &args) {
  const int32_t n = args.get_n();
  const int32_t m = args.get_m();
  const double rho = args.get_correlation();
  const int64_t seed = args.get_seed();
  const double weight_factor = args.get_weight_factor();
  const std::string file_path = args.get_folder_path() + "/" + args.get_outfile();
  const std::string r_script_path = "../scripts/generator.R";
  const std::string rho_str = fmt::format("{:.2f}", rho);

  const std::string command = fmt::format("./{} {} {} {} {} {} {} {}", r_script_path, n, m, rho, 0, weight_factor, seed, file_path);
  system(command.c_str());

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

  auto problem_solutions = solve_mobkp(args, points);
  auto problem = problem_solutions.first;
  auto solutions = problem_solutions.second;

  write_solution(args.get_folder_path(), args.get_outfile(), problem, solutions);
}

#endif  // SOLVER_HPP

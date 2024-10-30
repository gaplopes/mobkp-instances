#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <parser.hpp>
#include <solver.hpp>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    Arguments::print_usage();
    exit(1);
  }

  Arguments args(argc, argv);

  switch(args.get_type()) {
    case 0:
      generate_random_mobkp_test(args);
      break;
    case 1:
      generate_corr_mobkp_test(args);
      break;
    case 2:
      generate_corr_mobkp_test(args);
      break;
  }

  return 0;
}

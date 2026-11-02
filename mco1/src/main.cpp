#include <exception>
#include <iostream>
#include <string>

#include "cli.hpp"

int main(int argc, char* argv[]) {
  try {
    process_scheduler::cli::CLI cli(argc > 1 ? argv[1] : "config.txt");

    cli.run();

    return 0;
  } catch (const std::exception& err) {
    std::cerr << "Fatal error: " << err.what() << '\n';

    return 1;
  }
}

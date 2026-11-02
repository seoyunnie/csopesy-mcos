#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <cstdint>
#include <string>

namespace process_scheduler::config {

enum class Algorithm {
  /** First Come First Serve */
  FCFS,
  /** Round Robin */
  RR
};

struct Config {
  std::uint64_t num_cpu = 4;
  Algorithm scheduler = Algorithm::RR;
  std::uint64_t quantum_cycles = 5;
  std::uint64_t batch_process_freq = 1;
  std::uint64_t min_ins = 1000;
  std::uint64_t max_ins = 2000;
  std::uint64_t delays_per_exec = 0;

  static Config load(const std::string& filename);
};

}  // namespace process_scheduler::config

#endif  // CONFIG_HPP_

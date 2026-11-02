#include "config.hpp"

#include <charconv>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <unordered_map>

namespace {

constexpr std::uint64_t MAX_CPU_COUNT = 128;
constexpr std::uint64_t MAX_CONFIG_VALUE = std::uint64_t{1} << 32;

std::uint64_t parse_integer(std::string_view val) {
  std::uint64_t res{};

  const auto [ptr, err_code] = std::from_chars(val.data(), val.data() + val.size(), res);

  if (err_code != std::errc{} || ptr != val.data() + val.size()) {
    throw std::invalid_argument(std::format("Invalid 64-bit unsigned integer {}", val));
  }

  return res;
}

void validate_range(std::string_view name, std::uint64_t val, std::uint64_t min, std::uint64_t max) {
  if (val < min || val > max) {
    throw std::out_of_range(std::format("{} must be between {} and {}, got {}", name, min, max, val));
  }
}

}  // namespace

namespace process_scheduler::config {

Config Config::load(const std::string& filename) {
  std::ifstream in(filename);

  if (!in) {
    throw std::runtime_error("Unable to open " + filename);
  }

  std::unordered_map<std::string, std::string> vals;

  std::string line;

  while (std::getline(in, line)) {
    std::istringstream in_stream(line);

    std::string key;
    std::string val;

    if (!(in_stream >> key)) {
      continue;
    }

    if (!(in_stream >> val)) {
      throw std::runtime_error("Missing value for configuration key " + key);
    }

    if (vals.contains(key)) {
      throw std::runtime_error("Duplicate configuration key " + key);
    }

    vals.emplace(key, val);
  }

  Config config;

  const auto read_config_int = [&](std::string_view name, std::uint64_t fallback, std::uint64_t min,
                                   std::uint64_t max) {
    const auto iter = vals.find(std::string(name));

    if (iter == vals.end()) {
      return fallback;
    }

    const std::uint64_t val = parse_integer(iter->second);

    validate_range(name, val, min, max);

    return val;
  };

  config.num_cpu = read_config_int("num-cpu", config.num_cpu, 1, MAX_CPU_COUNT);
  config.quantum_cycles = read_config_int("quantum-cycles", config.quantum_cycles, 1, MAX_CONFIG_VALUE);
  config.batch_process_freq = read_config_int("batch-process-freq", config.batch_process_freq, 1, MAX_CONFIG_VALUE);
  config.min_ins = read_config_int("min-ins", config.min_ins, 1, MAX_CONFIG_VALUE);
  config.max_ins = read_config_int("max-ins", config.max_ins, 1, MAX_CONFIG_VALUE);
  config.delays_per_exec = read_config_int("delays-per-exec", config.delays_per_exec, 0, MAX_CONFIG_VALUE);

  if (const auto iter = vals.find("scheduler"); iter != vals.end()) {
    std::string algo = iter->second;

    if (algo.size() >= 2 && algo.front() == '"' && algo.back() == '"') {
      algo = algo.substr(1, algo.size() - 2);
    }

    if (algo == "fcfs") {
      config.scheduler = Algorithm::FCFS;
    } else if (algo == "rr") {
      config.scheduler = Algorithm::RR;
    } else {
      throw std::invalid_argument("scheduler must be either 'fcfs' or 'rr', got " + algo);
    }
  }

  if (config.min_ins > config.max_ins) {
    throw std::invalid_argument(
      std::format("min-ins must be less than or equal to max-ins, got {} and {}", config.min_ins, config.max_ins));
  }

  return config;
}

}  // namespace process_scheduler::config

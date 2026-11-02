#include "cli.hpp"

#include <exception>
#include <iostream>
#include <sstream>
#include <utility>

namespace process_scheduler::cli {

CLI::CLI(std::string config_file) : config_file_(std::move(config_file)) {}

void CLI::run() {
  std::cout << "Welcome to CSOPESY Emulator!\n"
            << '\n'
            << "Developers:\n"
            << "Panaligan, Louis Raphael\n"
            << "Lopez, Kent Xavier\n"
            << '\n'
            << "Last updated: 2026-11-03\n";

  std::string line;

  while (true) {
    std::cout << '\n';

    std::cout << "root:\\> ";

    if (!std::getline(std::cin, line)) {
      break;
    }

    const std::vector<std::string> args = tokenize(line);

    if (args.empty()) {
      continue;
    }

    const std::string& cmd = args.front();

    if (cmd == "exit" && args.size() == 1) {
      break;
    }

    if (!is_initialized_) {
      if (cmd == "initialize" && args.size() == 1) {
        initialize();
      } else {
        std::cout << "Error: configuration not initialized\n";
      }

      continue;
    }

    if (cmd == "screen") {
      handle_screen(args);
    } else if (cmd == "scheduler-start" && args.size() == 1) {
      scheduler_->start_generating();

      std::cout << "Scheduler process generation started\n";
    } else if (cmd == "scheduler-stop" && args.size() == 1) {
      scheduler_->stop_generating();

      std::cout << "Scheduler process generation stopped\n";
    } else if (cmd == "report-util" && args.size() == 1) {
      if (scheduler_->write_report("csopesy-log.txt")) {
        std::cout << "Report generated at 'csopesy-log.txt'\n";
      } else {
        std::cout << "Error: unable to write 'csopesy-log.txt'\n";
      }
    } else if (cmd == "initialize" && args.size() == 1) {
      std::cout << "Error: configuration already initialized\n";
    } else {
      std::cout << "Error: unknown command input\n";
    }
  }

  if (scheduler_) {
    scheduler_->stop();
  }

  std::cout << "Terminating emulator...\n";
}

std::vector<std::string> CLI::tokenize(const std::string& line) {
  std::istringstream in(line);

  std::vector<std::string> args;

  std::string token;

  while (in >> token) {
    args.push_back(token);
  }

  return args;
}

void CLI::initialize() {
  try {
    const config::Config config = config::Config::load(config_file_);

    scheduler_ = std::make_unique<scheduler::Scheduler>(config);

    scheduler_->start();

    is_initialized_ = true;

    std::cout << "Configuration file loaded successfully\n"
              << "  CPU cores: " << config.num_cpu << '\n'
              << "  Scheduler: " << (config.scheduler == config::Algorithm::FCFS ? "FCFS" : "RR") << '\n'
              << "  Quantum cycles: " << config.quantum_cycles << '\n'
              << "  Batch process frequency: " << config.batch_process_freq << '\n'
              << "  Minimum instructions: " << config.min_ins << '\n'
              << "  Maximum instructions: " << config.max_ins << '\n'
              << "  Delays per execution: " << config.delays_per_exec << '\n';
  } catch (const std::exception& err) {
    std::cout << "Error: " << err.what() << '\n';
  }
}

void CLI::handle_screen(const std::vector<std::string>& args) {
  if (args.size() == 2 && args[1] == "-ls") {
    std::cout << scheduler_->status_report();

    return;
  }

  if (args.size() != 3 || (args[1] != "-s" && args[1] != "-r")) {
    std::cout << "Usage: screen -s <process-name> | screen -r <process-name> | screen -ls\n";

    return;
  }

  if (args[1] == "-s") {
    if (!scheduler_->create_process(args[2])) {
      std::cout << "Error: a process with the name " << args[2] << " already exists\n";
    }

    return;
  }

  if (scheduler_->is_process_finished(args[2])) {
    std::cout << "Error: process " << args[2] << " is finished\n";

    return;
  }

  process_screen(args[2]);
}

void CLI::process_screen(const std::string& name) {
  std::string line;

  while (true) {
    std::cout << '\n';

    std::cout << "root:\\" << name << "> ";

    if (!std::getline(std::cin, line)) {
      return;
    }

    const std::vector<std::string> args = tokenize(line);

    if (args.empty()) {
      continue;
    }

    const std::string& cmd = args.front();

    if (cmd == "exit" && args.size() == 1) {
      return;
    }

    if (cmd == "process-smi" && args.size() == 1) {
      scheduler_->print_process_info(name, std::cout);
    } else {
      std::cout << "Error: unknown command input\n";
    }
  }
}

}  // namespace process_scheduler::cli

#ifndef CLI_HPP_
#define CLI_HPP_

#include <memory>
#include <string>
#include <vector>

#include "scheduler.hpp"

namespace process_scheduler::cli {

class CLI {
 public:
  explicit CLI(std::string config_file);

  void run();

 private:
  std::string config_file_;

  bool is_initialized_ = false;

  std::unique_ptr<scheduler::Scheduler> scheduler_;

  static std::vector<std::string> tokenize(const std::string& line);

  void initialize();

  void handle_screen(const std::vector<std::string>& args);

  void process_screen(const std::string& name);
};

}  // namespace process_scheduler::cli

#endif  // CLI_HPP_

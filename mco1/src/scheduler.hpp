#ifndef SCHEDULER_HPP_
#define SCHEDULER_HPP_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "config.hpp"
#include "process.hpp"

namespace process_scheduler::scheduler {

class Scheduler {
 public:
  explicit Scheduler(config::Config config);

  ~Scheduler();

  Scheduler(const Scheduler&) = delete;

  Scheduler& operator=(const Scheduler&) = delete;

  std::optional<std::string> screened_process_name_;

  void set_screened_process(std::optional<std::string> name);

  void start();

  void stop();

  void start_generating();

  void stop_generating();

  [[nodiscard]] bool is_generating() const;

  [[nodiscard]] bool contains_process(const std::string& name) const;

  [[nodiscard]] bool is_process_finished(const std::string& name) const;

  bool create_process(const std::string& name);

  void print_process_info(const std::string& name, std::ostream& output) const;

  [[nodiscard]] std::string status_report() const;

  bool write_report(const std::string& filename) const;

  [[nodiscard]] std::uint64_t tick_count() const;

 private:
  config::Config config_;

  mutable std::mutex mutex_;

  std::vector<process::CPUCore> cores_;
  std::vector<std::shared_ptr<process::Process>> processes_;
  std::queue<std::uint64_t> ready_queue_;

  std::mt19937 random_engine_;

  std::thread worker_;

  bool is_shutting_down_ = false;
  bool is_generating_ = false;

  std::uint64_t tick_ = 0;
  std::uint64_t next_id_ = 1;
  std::uint64_t next_generated_name_ = 1;

  std::uint64_t total_core_cycles_ = 0;

  static std::string timestamp_now();

  std::shared_ptr<process::Process> find_by_name_locked(const std::string& name) const;

  std::shared_ptr<process::Process> find_by_id_locked(std::uint64_t id) const;

  std::shared_ptr<process::Process> create_process_locked(const std::string& name);

  void generate_program(process::Process& process);

  void append_random_instruction(process::Process& process, std::uint64_t depth);

  static std::uint16_t operand_value(process::Process& process, const std::string& operand);

  void execute_one(process::Process& process, std::size_t core);

  void finish(process::Process& process);

  void release_core(std::size_t core);

  std::shared_ptr<process::Process> pop_ready_locked();

  void run();
};

}  // namespace process_scheduler::scheduler

#endif  // SCHEDULER_HPP_

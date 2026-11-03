#include "scheduler.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <utility>

namespace process_scheduler::scheduler {

using namespace std::chrono_literals;

Scheduler::Scheduler(config::Config config)
    : config_(std::move(config)),
      cores_(static_cast<std::size_t>(config_.num_cpu)),
      random_engine_(std::random_device{}()) {}

Scheduler::~Scheduler() {
  stop();
}

void Scheduler::start() {
  std::lock_guard lock(mutex_);

  if (worker_.joinable()) {
    return;
  }

  is_shutting_down_ = false;

  worker_ = std::thread(&Scheduler::run, this);
}

void Scheduler::stop() {
  {
    std::lock_guard lock(mutex_);

    is_shutting_down_ = true;
  }

  if (worker_.joinable()) {
    worker_.join();
  }
}

void Scheduler::start_generating() {
  std::lock_guard lock(mutex_);

  is_generating_ = true;
}

void Scheduler::stop_generating() {
  std::lock_guard lock(mutex_);

  is_generating_ = false;
}

bool Scheduler::is_generating() const {
  std::lock_guard lock(mutex_);

  return is_generating_;
}

bool Scheduler::contains_process(const std::string& name) const {
  std::lock_guard lock(mutex_);

  return static_cast<bool>(find_by_name_locked(name));
}

bool Scheduler::is_process_finished(const std::string& name) const {
  std::lock_guard lock(mutex_);

  const std::shared_ptr<process::Process> process = find_by_name_locked(name);

  return !process || process->finished();
}

bool Scheduler::create_process(const std::string& name) {
  std::lock_guard lock(mutex_);

  if (find_by_name_locked(name)) {
    return false;
  }

  create_process_locked(name);

  return true;
}

void Scheduler::print_process_info(const std::string& name, std::ostream& out) const {
  std::lock_guard lock(mutex_);

  const std::shared_ptr<process::Process> process = find_by_name_locked(name);

  if (!process) {
    out << "Error: process " << name << " not found\n";

    return;
  }

  out << "Process name: " << process->name << '\n' << "ID: " << process->id << '\n' << "Logs:\n";

  for (const auto& log : process->logs) {
    out << "  [" << log.timestamp << "] Core: " << log.core << " \"" << log.message << "\"\n";
  }

  out << '\n';

  if (process->finished()) {
    out << "Finished!\n";
  } else {
    out << "Current instruction line: " << std::min(process->program_counter, process->code.size()) << '\n'
        << "Lines of code: " << process->code.size() << '\n';
  }
}

std::string Scheduler::status_report() const {
  std::lock_guard lock(mutex_);

  std::ostringstream out;

  const double utilization_percentage =
    total_core_cycles_ == 0 ? 0.0
                            : 100.0 * static_cast<double>(busy_core_cycles_) / static_cast<double>(total_core_cycles_);
  const auto used_core_cnt =
    std::count_if(cores_.begin(), cores_.end(), [](const process::CPUCore& c) { return c.process_id.has_value(); });

  out << std::fixed << std::setprecision(2) << "CPU utilization: " << utilization_percentage << "%\n"
      << "Cores used: " << used_core_cnt << '\n'
      << "Cores available: " << cores_.size() - used_core_cnt << "\n";

  out << '\n';

  out << "Running processes:\n";

  bool is_any_running = false;

  for (const auto& process : processes_) {
    if (process->state != process::ProcessState::RUNNING) {
      continue;
    }

    is_any_running = true;

    out << "  " << process->name << "  (" << process->created_at
        << ")  Core: " << (process->last_core ? std::to_string(*process->last_core) : "-") << "  "
        << process->program_counter << " / " << process->code.size() << '\n';
  }

  if (!is_any_running) {
    out << "  (none)\n";
  }

  out << '\n';

  out << "Ready/blocked processes:\n";

  bool is_any_waiting = false;

  for (const auto& process : processes_) {
    if (process->state != process::ProcessState::READY && process->state != process::ProcessState::BLOCKED) {
      continue;
    }

    is_any_waiting = true;

    out << "  " << process->name << "  (" << process->created_at << ")  "
        << (process->state == process::ProcessState::READY ? "Ready" : "Blocked") << "  " << process->program_counter
        << " / " << process->code.size() << '\n';
  }

  if (!is_any_waiting) {
    out << "  (none)\n";
  }

  out << '\n';

  out << "Finished processes:\n";

  bool is_any_finished = false;

  for (const auto& process : processes_) {
    if (!process->finished()) {
      continue;
    }

    is_any_finished = true;

    out << "  " << process->name << "  (" << process->created_at << ")  Finished  " << process->code.size() << " / "
        << process->code.size() << '\n';
  }

  if (!is_any_finished) {
    out << "  (none)\n";
  }

  return out.str();
}

bool Scheduler::write_report(const std::string& filename) const {
  std::ofstream out(filename);

  if (!out) {
    return false;
  }

  out << status_report();

  return static_cast<bool>(out);
}

std::uint64_t Scheduler::tick_count() const {
  std::lock_guard lock(mutex_);

  return tick_;
}

std::string Scheduler::timestamp_now() {
  const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  std::tm local_time{};

#ifdef _WIN32
  localtime_s(&local_time, &now);
#else
  localtime_r(&now, &local_time);
#endif

  std::ostringstream out;

  out << std::put_time(&local_time, "%m/%d/%Y %H:%M:%S");

  return out.str();
}

std::shared_ptr<process::Process> Scheduler::find_by_name_locked(const std::string& name) const {
  const auto iter = std::find_if(processes_.begin(), processes_.end(), [&](const auto& p) { return p->name == name; });

  return iter == processes_.end() ? nullptr : *iter;
}

std::shared_ptr<process::Process> Scheduler::find_by_id_locked(std::uint64_t id) const {
  const auto iter = std::find_if(processes_.begin(), processes_.end(), [id](const auto& p) { return p->id == id; });

  return iter == processes_.end() ? nullptr : *iter;
}

std::shared_ptr<process::Process> Scheduler::create_process_locked(const std::string& name) {
  auto process = std::make_shared<process::Process>();

  process->id = next_id_++;
  process->name = name;
  process->created_tick = tick_;
  process->created_at = timestamp_now();

  generate_program(*process);

  processes_.push_back(process);

  ready_queue_.push(process->id);

  return process;
}

void Scheduler::generate_program(process::Process& process) {
  std::uniform_int_distribution<std::uint64_t> cnt_dist(config_.min_ins, config_.max_ins);

  const std::uint64_t instruction_cnt = cnt_dist(random_engine_);

  for (std::uint64_t i = 0; i < instruction_cnt; ++i) {
    append_random_instruction(process, 0);
  }
}

void Scheduler::append_random_instruction(process::Process& process, std::uint64_t depth) {
  std::uniform_int_distribution<std::uint32_t> op_dist(0, depth < 3 ? 5 : 4);

  std::uniform_int_distribution<std::uint32_t> val_dist(0, 100);
  std::uniform_int_distribution<std::uint32_t> sleep_dist(1, 8);

  const std::uint32_t choice = op_dist(random_engine_);

  if (choice == 5) {
    std::uniform_int_distribution<std::uint32_t> repeat_dist(1, 2);
    std::uniform_int_distribution<std::uint32_t> body_dist(1, 2);

    const std::uint32_t repeats = repeat_dist(random_engine_);
    const std::uint32_t body_size = body_dist(random_engine_);

    for (std::uint32_t repeat = 0; repeat < repeats; ++repeat) {
      for (std::uint32_t i = 0; i < body_size; ++i) {
        append_random_instruction(process, depth + 1);
      }
    }

    return;
  }

  const std::string first = "v" + std::to_string(val_dist(random_engine_) % 8);
  const std::string second = "v" + std::to_string(val_dist(random_engine_) % 8);
  const std::string third = "v" + std::to_string(val_dist(random_engine_) % 8);

  process::Instruction instruction;

  switch (choice) {
    case 0:
      instruction.operation = process::Operation::PRINT;

      instruction.message = "Hello world from " + process.name + "!";

      break;
    case 1:
      instruction.operation = process::Operation::DECLARE;

      instruction.target = first;

      instruction.value = static_cast<std::uint16_t>(val_dist(random_engine_));

      break;
    case 2:
    case 3:
      instruction.operation = choice == 2 ? process::Operation::ADD : process::Operation::SUBTRACT;

      instruction.target = first;
      instruction.lhs = second;
      instruction.rhs = val_dist(random_engine_) % 2 == 0 ? std::to_string(val_dist(random_engine_) % 100) : third;

      break;
    default:
      instruction.operation = process::Operation::SLEEP;

      instruction.sleep_ticks = sleep_dist(random_engine_);

      break;
  }

  process.code.push_back(std::move(instruction));
}

std::uint16_t Scheduler::operand_value(process::Process& process, const std::string& operand) {
  if (operand.empty()) {
    return 0;
  }

  const bool is_numeric =
    std::all_of(operand.begin(), operand.end(), [](unsigned char c) { return std::isdigit(c) != 0; });

  if (is_numeric) {
    return static_cast<std::uint16_t>(std::min<unsigned long>(std::stoul(operand), 65535));
  }

  return process.variables.try_emplace(operand, static_cast<std::uint16_t>(0)).first->second;
}

void Scheduler::execute_one(process::Process& process, std::size_t core) {
  if (process.program_counter >= process.code.size()) {
    finish(process);

    return;
  }

  const process::Instruction& instruction = process.code[process.program_counter++];

  process.last_core = core;

  switch (instruction.operation) {
    case process::Operation::PRINT:
      process.logs.push_back({tick_, core, timestamp_now(), instruction.message});

      break;
    case process::Operation::DECLARE:
      process.variables[instruction.target] = instruction.value;

      break;
    case process::Operation::ADD: {
      const std::uint32_t result = static_cast<std::uint32_t>(operand_value(process, instruction.lhs)) +
                                   static_cast<std::uint32_t>(operand_value(process, instruction.rhs));

      process.variables[instruction.target] = static_cast<std::uint16_t>(std::min(result, 65535u));

      break;
    }
    case process::Operation::SUBTRACT: {
      const std::int32_t result = static_cast<std::int32_t>(operand_value(process, instruction.lhs)) -
                                  static_cast<std::int32_t>(operand_value(process, instruction.rhs));

      process.variables[instruction.target] = static_cast<std::uint16_t>(std::clamp(result, 0, 65535));

      break;
    }
    case process::Operation::SLEEP:
      process.state = process::ProcessState::BLOCKED;

      process.sleep_remaining = instruction.sleep_ticks;

      release_core(core);

      break;
  }

  if (process.state != process::ProcessState::BLOCKED && process.program_counter >= process.code.size()) {
    finish(process);
  } else if (process.state == process::ProcessState::RUNNING) {
    process.delay_remaining = config_.delays_per_exec;
  }
}

void Scheduler::finish(process::Process& process) {
  process.state = process::ProcessState::FINISHED;

  process.finished_tick = tick_;

  for (auto& core : cores_) {
    if (core.process_id == process.id) {
      core.process_id.reset();
    }
  }
}

void Scheduler::release_core(std::size_t core_idx) {
  if (core_idx >= cores_.size() || !cores_[core_idx].process_id) {
    return;
  }

  const std::shared_ptr<process::Process> process = find_by_id_locked(*cores_[core_idx].process_id);

  cores_[core_idx].process_id.reset();

  if (process && process->state == process::ProcessState::RUNNING) {
    process->state = process::ProcessState::READY;

    process->quantum_used = 0;

    ready_queue_.push(process->id);
  }
}

std::shared_ptr<process::Process> Scheduler::pop_ready_locked() {
  while (!ready_queue_.empty()) {
    const std::uint64_t id = ready_queue_.front();

    ready_queue_.pop();

    const std::shared_ptr<process::Process> process = find_by_id_locked(id);

    if (process && process->state == process::ProcessState::READY) {
      return process;
    }
  }

  return nullptr;
}

void Scheduler::run() {
  while (true) {
    {
      std::lock_guard lock(mutex_);

      if (is_shutting_down_) {
        break;
      }

      ++tick_;

      if (is_generating_ && (tick_ == 1 || tick_ % config_.batch_process_freq == 0)) {
        std::ostringstream name;

        do {
          name.str({});
          name.clear();

          name << "process" << std::setw(2) << std::setfill('0') << next_generated_name_++;

        } while (find_by_name_locked(name.str()));

        create_process_locked(name.str());
      }

      for (auto& process : processes_) {
        if (process->state == process::ProcessState::BLOCKED && process->sleep_remaining > 0) {
          --process->sleep_remaining;

          if (process->sleep_remaining == 0) {
            process->state = process::ProcessState::READY;

            ready_queue_.push(process->id);
          }
        }
      }

      for (std::size_t core_idx = 0; core_idx < cores_.size(); ++core_idx) {
        auto& core = cores_[core_idx];

        if (!core.process_id) {
          const std::shared_ptr<process::Process> next_process = pop_ready_locked();

          if (next_process) {
            core.process_id = next_process->id;

            next_process->state = process::ProcessState::RUNNING;

            next_process->quantum_used = 0;

            next_process->last_core = core_idx;
          }
        }

        if (!core.process_id) {
          continue;
        }

        const std::shared_ptr<process::Process> process = find_by_id_locked(*core.process_id);

        if (!process || process->state != process::ProcessState::RUNNING) {
          core.process_id.reset();

          continue;
        }

        if (process->delay_remaining > 0) {
          --process->delay_remaining;
        } else {
          execute_one(*process, core_idx);

          if (core.process_id && process->state == process::ProcessState::RUNNING &&
              config_.scheduler == config::Algorithm::RR) {
            ++process->quantum_used;

            if (process->quantum_used >= config_.quantum_cycles) {
              release_core(core_idx);
            }
          }
        }
      }

      total_core_cycles_ += cores_.size();
    }

    std::this_thread::sleep_for(10ms);
  }
}

}  // namespace process_scheduler::scheduler

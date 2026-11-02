#ifndef PROCESS_HPP_
#define PROCESS_HPP_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace process_scheduler::process {

enum class ProcessState { READY, RUNNING, BLOCKED, FINISHED };

enum class Operation { PRINT, DECLARE, ADD, SUBTRACT, SLEEP };

struct Instruction {
  Operation operation{};

  std::string target;
  std::string lhs;
  std::string rhs;
  std::string message;

  std::uint16_t value = 0;
  std::uint64_t sleep_ticks = 0;
};

struct LogEntry {
  std::uint64_t tick = 0;
  std::size_t core = 0;

  std::string timestamp;
  std::string message;
};

struct Process {
  std::uint64_t id = 0;
  std::string name;

  std::vector<Instruction> code;
  std::unordered_map<std::string, std::uint16_t> variables;

  std::vector<LogEntry> logs;

  std::size_t program_counter = 0;

  ProcessState state = ProcessState::READY;

  std::uint64_t created_tick = 0;
  std::string created_at;

  std::optional<std::uint64_t> finished_tick;

  std::uint64_t sleep_remaining = 0;
  std::uint64_t delay_remaining = 0;
  std::uint64_t quantum_used = 0;

  std::optional<std::size_t> last_core;

  [[nodiscard]] bool finished() const noexcept {
    return state == ProcessState::FINISHED;
  }
};

struct CPUCore {
  std::optional<std::uint64_t> process_id;
};

}  // namespace process_scheduler::process

#endif  // PROCESS_HPP_

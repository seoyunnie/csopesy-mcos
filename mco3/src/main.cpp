#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

constexpr char ANSI_ALTERNATE_SCREEN_BUFFER[] = "\u001B[?1049h";
constexpr char ANSI_MAIN_SCREEN_BUFFER[] = "\u001B[?1049l";

constexpr char ANSI_CLEAR_SCREEN[] = "\x1B[2J";

constexpr char ANSI_CURSOR_SAVE[] = "\x1B[s";
constexpr char ANSI_CURSOR_RESTORE[] = "\x1B[u";
constexpr char ANSI_CURSOR_UP[] = "\x1B[A";
constexpr char ANSI_CURSOR_DOWN[] = "\x1B[B";
constexpr char ANSI_CURSOR_HOME[] = "\x1B[H";
constexpr char ANSI_ERASE_LINE[] = "\x1B[2K";

void initialize_console() {
  std::cout << ANSI_ALTERNATE_SCREEN_BUFFER << ANSI_CLEAR_SCREEN << ANSI_CURSOR_HOME;
}

void restore_console() {
  std::cout << ANSI_MAIN_SCREEN_BUFFER;
}

bool is_cursor_escape_sequence(const std::string& in) {
  return in == ANSI_CURSOR_UP || in == ANSI_CURSOR_DOWN;
}

constexpr char DEFAULT_MARQUEE_TEXT[] = "Welcome to CSOPESY!";
constexpr int DEFAULT_MARQUEE_SPEED = 250;

std::mutex marquee_mutex;
std::condition_variable marquee_cv;

bool is_marquee_running = false;
bool is_shutting_down = false;

int marquee_speed = DEFAULT_MARQUEE_SPEED;
std::string marquee_text = DEFAULT_MARQUEE_TEXT;

void display_marquee(const std::string& text, std::size_t pos) {
  if (text.empty()) {
    return;
  }

  const std::string display_text = text.substr(pos) + " " + text.substr(0, pos);

  std::cout << ANSI_CURSOR_SAVE << ANSI_CURSOR_HOME << ANSI_ERASE_LINE << display_text << ANSI_CURSOR_RESTORE
            << std::flush;
}

void marquee_loop() {
  std::size_t pos = 0;

  std::unique_lock lock(marquee_mutex);

  while (!is_shutting_down) {
    marquee_cv.wait(lock, [] { return is_marquee_running || is_shutting_down; });

    if (is_shutting_down) {
      break;
    }

    const int speed = marquee_speed;
    const std::string text = marquee_text;

    lock.unlock();

    if (!text.empty()) {
      pos %= text.size();

      display_marquee(text, pos);

      pos = (pos + 1) % text.size();
    } else {
      pos = 0;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(speed));

    lock.lock();
  }
}

void shutdown_marquee(std::thread& thread) {
  {
    std::lock_guard lock(marquee_mutex);

    is_shutting_down = true;
    is_marquee_running = false;
  }

  marquee_cv.notify_one();
  thread.join();
}

int main() {
  initialize_console();

  std::thread marquee_thread(marquee_loop);

  std::cout << '\n';

  std::cout << '\n'
            << "Group developer:\n"
            << "Panaligan, Louis Raphael\n"
            << "Lopez, Kent Xavier\n"
            << '\n'
            << "Version date: 2026-09-20\n";

  std::cout << '\n';

  std::string in;

  while (true) {
    std::cout << "Command> ";

    if (!std::getline(std::cin, in)) {
      shutdown_marquee(marquee_thread);

      restore_console();

      return 1;
    }

    // macOS terminal scrolling behavior fix.
    if (is_cursor_escape_sequence(in)) {
      continue;
    }

    std::istringstream stream(in);

    std::string cmd;
    stream >> cmd;

    if (cmd == "help") {
      std::cout << "help - displays the commands and its description\n"
                << "start_marquee - starts the marquee \"animation\"\n"
                << "stop_marquee - stops the marquee \"animation\"\n"
                << "set_text - accepts a text input and displays it as a marquee\n"
                << "set_speed - sets the marquee animation refresh in milliseconds\n"
                << "exit - terminates the console\n";

      std::cout << '\n';
    } else if (cmd == "start_marquee") {
      {
        std::lock_guard lock(marquee_mutex);

        if (is_marquee_running) {
          std::cout << "Error: marquee is already running\n";

          std::cout << '\n';

          continue;
        }

        is_marquee_running = true;
      }

      marquee_cv.notify_one();

      std::cout << "Marquee started\n";

      std::cout << '\n';
    } else if (cmd == "stop_marquee") {
      {
        std::lock_guard lock(marquee_mutex);

        if (!is_marquee_running) {
          std::cout << "Error: marquee is not running\n";

          std::cout << '\n';

          continue;
        }

        is_marquee_running = false;
      }

      std::cout << "Marquee stopped\n";

      std::cout << '\n';
    } else if (cmd == "set_text") {
      std::string text;
      std::getline(stream >> std::ws, text);

      if (text.empty()) {
        std::cout << "Error: set_text requires a string argument\n";

        std::cout << '\n';

        continue;
      }

      {
        std::lock_guard lock(marquee_mutex);

        marquee_text = text;
      }

      std::cout << "Text saved for marquee: " << text << '\n';

      std::cout << '\n';
    } else if (cmd == "set_speed") {
      int speed;

      if (!(stream >> speed)) {
        std::cout << "Error: set_speed requires a millisecond value\n";

        std::cout << '\n';

        continue;
      }

      if (speed <= 0) {
        std::cout << "Error: set_speed requires a positive millisecond value\n";

        std::cout << '\n';

        continue;
      }

      {
        std::lock_guard lock(marquee_mutex);

        marquee_speed = speed;
      }

      std::cout << "Marquee speed set to " << speed << " ms\n";

      std::cout << '\n';
    } else if (cmd == "exit") {
      std::cout << "Terminating console...\n";

      break;
    } else if (!cmd.empty()) {
      std::cout << "Error: unknown command input\n";

      std::cout << '\n';
    }
  }

  shutdown_marquee(marquee_thread);

  restore_console();

  return 0;
}

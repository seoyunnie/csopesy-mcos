#include "clock.hpp"

#include <chrono>
#include <ctime>

namespace {

std::tm get_local_time() {
  const std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

  std::tm local_time{};

#ifdef _WIN32
  localtime_s(&local_time, &now);
#else
  localtime_r(&now, &local_time);
#endif

  return local_time;
}

}  // namespace

namespace os_mockup::clock {

std::string format_time() {
  const std::tm local_time = get_local_time();

  char buff[32]{};

  std::strftime(buff, sizeof(buff), "%I:%M %p", &local_time);

  return buff;
}

std::string format_date() {
  const std::tm local_time = get_local_time();

  char buff[32]{};

  std::strftime(buff, sizeof(buff), "%m/%d/%Y", &local_time);

  return buff;
}

}  // namespace os_mockup::clock

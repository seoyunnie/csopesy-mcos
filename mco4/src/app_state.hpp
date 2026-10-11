#ifndef APP_STATE_HPP_
#define APP_STATE_HPP_

namespace os_mockup {

struct AppState {
  bool is_file_explorer_open = false;
  bool is_browser_open = false;
  bool is_task_manager_open = false;
  bool is_shutting_down = false;
};

}  // namespace os_mockup

#endif  // APP_STATE_HPP_

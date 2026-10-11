#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>

#include "app_state.hpp"
#include "apps.hpp"
#include "clock.hpp"
#include "desktop.hpp"
#include "style.hpp"

namespace {

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;

}  // namespace

int main() {
  if (!glfwInit()) {
    std::fprintf(stderr, "Failed to initialize GLFW.\n");

    return 1;
  }

#ifdef __APPLE__
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

  GLFWwindow* win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OS Mockup", nullptr, nullptr);

  if (win == nullptr) {
    std::fprintf(stderr, "Failed to create the GLFW window.\n");

    glfwTerminate();

    return 1;
  }

  glfwSetWindowCloseCallback(win, [](GLFWwindow* w) { glfwSetWindowShouldClose(w, GLFW_FALSE); });
  glfwMakeContextCurrent(win);
  glfwSwapInterval(1);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;

  os_mockup::style::setup();

  if (!ImGui_ImplGlfw_InitForOpenGL(win, true)) {
    std::fprintf(stderr, "Failed to initialize ImGui GLFW backend.\n");

    ImGui::DestroyContext();

    glfwDestroyWindow(win);
    glfwTerminate();

    return 1;
  }

  if (!ImGui_ImplOpenGL3_Init("#version 150")) {
    std::fprintf(stderr, "Failed to initialize ImGui OpenGL backend.\n");

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    glfwDestroyWindow(win);
    glfwTerminate();

    return 1;
  }

  os_mockup::AppState app;

  while (!glfwWindowShouldClose(win) && !app.is_shutting_down) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    os_mockup::desktop::draw_wallpaper();

    os_mockup::apps::draw_files_app(app.is_file_explorer_open);
    os_mockup::apps::draw_browser_app(app.is_browser_open);
    os_mockup::apps::draw_task_manager(app.is_task_manager_open);

    os_mockup::desktop::draw_taskbar(app);

    ImGui::Render();

    int frame_buff_w = 0;
    int frame_buff_h = 0;

    glfwGetFramebufferSize(win, &frame_buff_w, &frame_buff_h);

    glViewport(0, 0, frame_buff_w, frame_buff_h);

    glClearColor(0.03f, 0.05f, 0.09f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(win);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();

  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}

#include "apps.hpp"

#include <imgui.h>

#include <array>

namespace {

struct Process {
  const char* name;
  const char* type;
  float cpu;
  float memory;
};

constexpr std::array<Process, 7> PROCESSES{{
  {"Desktop Window Manager", "System", 2.1f, 142.0f},
  {"File Explorer", "App", 0.4f, 85.0f},
  {"Web Browser", "App", 5.8f, 512.0f},
  {"System Settings", "App", 0.2f, 96.0f},
  {"Search Host", "Background", 0.1f, 42.0f},
  {"Windows Audio", "System", 0.0f, 18.0f},
  {"Runtime Broker", "Background", 0.3f, 37.0f},
}};

}  // namespace

namespace os_mockup::apps {

void draw_files_app(bool& open) {
  if (!open) {
    return;
  }

  ImGui::SetNextWindowSize(ImVec2(600.0f, 420.0f), ImGuiCond_FirstUseEver);

  if (ImGui::Begin("File Explorer", &open)) {
    ImGui::Text("Home");

    ImGui::Separator();

    ImGui::TextDisabled("Quick access");

    if (ImGui::Button("Desktop", ImVec2(150.0f, 36.0f))) {
    }

    ImGui::SameLine();

    if (ImGui::Button("Documents", ImVec2(150.0f, 36.0f))) {
    }

    ImGui::SameLine();

    if (ImGui::Button("Downloads", ImVec2(150.0f, 36.0f))) {
    }

    ImGui::Spacing();

    ImGui::Separator();

    ImGui::Text("Name");

    ImGui::Separator();

    ImGui::BulletText("Desktop");
    ImGui::BulletText("Documents");
    ImGui::BulletText("Downloads");
    ImGui::BulletText("Pictures");

    ImGui::Spacing();

    ImGui::TextDisabled("Placeholder interface. No files are accessed.");
  }

  ImGui::End();
}

void draw_browser_app(bool& open) {
  if (!open) {
    return;
  }

  ImGui::SetNextWindowSize(ImVec2(700.0f, 460.0f), ImGuiCond_FirstUseEver);

  if (ImGui::Begin("Web Browser", &open)) {
    static char addr[256] = "https://example.com";

    ImGui::SetNextItemWidth(-100.0f);

    ImGui::InputText("##Address", addr, sizeof(addr));

    ImGui::SameLine();

    ImGui::Button("Go", ImVec2(70.0f, 0.0f));

    ImGui::Separator();

    ImGui::Dummy(ImVec2(0.0f, 24.0f));

    const float w = ImGui::GetContentRegionAvail().x;
    const float h = ImGui::GetContentRegionAvail().y;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + w * 0.5f - 100.0f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + h * 0.25f);

    ImGui::Text("Welcome to the web");

    ImGui::TextDisabled("This is a placeholder browser screen.");
  }

  ImGui::End();
}

void draw_task_manager(bool& open) {
  if (!open) {
    return;
  }

  ImGui::SetNextWindowSize(ImVec2(820.0f, 510.0f), ImGuiCond_FirstUseEver);

  if (ImGui::Begin("Task Manager", &open)) {
    ImGui::Text("Processes");

    ImGui::SameLine();

    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 145.0f);

    ImGui::TextDisabled("Performance");

    ImGui::Separator();

    ImGui::Text("CPU");
    ImGui::SameLine(90.0f);
    ImGui::ProgressBar(0.23f, ImVec2(100.0f, 0.0f), "23%");

    ImGui::SameLine();

    ImGui::Text("Memory");
    ImGui::SameLine(385.0f);
    ImGui::ProgressBar(0.61f, ImVec2(100.0f, 0.0f), "61%");

    ImGui::Spacing();

    if (ImGui::BeginTable("Processes", 4,
                          ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY,
                          ImVec2(0.0f, 280.0f))) {
      ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
      ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
      ImGui::TableSetupColumn("CPU", ImGuiTableColumnFlags_WidthFixed, 80.0f);
      ImGui::TableSetupColumn("Memory (MB)", ImGuiTableColumnFlags_WidthFixed, 110.0f);
      ImGui::TableHeadersRow();

      for (const auto& process : PROCESSES) {
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(process.name);

        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(process.type);

        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%.1f%%", process.cpu);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%.1f", process.memory);
      }

      ImGui::EndTable();
    }

    ImGui::Spacing();

    ImGui::TextDisabled("Values are simulated and do not represent real system processes.");
  }

  ImGui::End();
}

}  // namespace os_mockup::apps

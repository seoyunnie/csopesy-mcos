#include "desktop.hpp"

#include <imgui.h>

#include <cstdint>

#include "clock.hpp"

namespace {

constexpr float TASKBAR_HEIGHT = 56.0f;

bool icon_button(const char* id, const os_mockup::texture::Texture& icon, const char* fallback_label,
                 const ImVec2& fallback_size) {
  if (icon.is_valid()) {
    return ImGui::ImageButton(id, ImTextureRef{static_cast<ImTextureID>(static_cast<std::intptr_t>(icon.id))},
                              ImVec2(26.0F, 24.0F));
  }

  return ImGui::Button(fallback_label, fallback_size);
}

}  // namespace

namespace os_mockup::desktop {

void draw_wallpaper(const texture::Texture& wallpaper) {
  ImDrawList* draw = ImGui::GetBackgroundDrawList();

  if (wallpaper.is_valid()) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    const ImVec2 pos = viewport->Pos;
    const ImVec2 size = viewport->Size;

    const ImVec2 end = ImVec2(pos.x + size.x, pos.y + size.y);

    draw->AddImage(ImTextureRef{static_cast<ImTextureID>(static_cast<std::intptr_t>(wallpaper.id))}, pos, end);
  } else {
    const ImVec2 size = ImGui::GetIO().DisplaySize;

    draw->AddRectFilledMultiColor(ImVec2(0.0f, 0.0f), size, IM_COL32(16, 42, 86, 255), IM_COL32(29, 83, 145, 255),
                                  IM_COL32(7, 24, 54, 255), IM_COL32(12, 39, 79, 255));

    const ImVec2 center(size.x * 0.57f, size.y * 0.43f);

    draw->AddCircleFilled(center, size.y * 0.34f, IM_COL32(42, 119, 191, 100), 96);
    draw->AddCircleFilled(ImVec2(center.x + 100.0f, center.y + 35.0f), size.y * 0.24f, IM_COL32(65, 158, 221, 90), 96);
    draw->AddCircleFilled(ImVec2(center.x - 90.0f, center.y + 70.0f), size.y * 0.18f, IM_COL32(22, 78, 147, 130), 96);
  }

  draw->AddText(ImVec2(32.0f, 28.0f), IM_COL32(255, 255, 255, 230), "CSOPESY");
  draw->AddText(ImVec2(32.0f, 51.0f), IM_COL32(220, 235, 255, 180), "Panaligan, Louis Raphael");
  draw->AddText(ImVec2(32.0f, 66.0f), IM_COL32(220, 235, 255, 180), "Lopez, Kent Xavier");
}

void draw_taskbar(AppState& app, const texture::Texture& file_explorer_icon, const texture::Texture& browser_icon,
                  const texture::Texture& task_manager_icon) {
  const ImVec2 display = ImGui::GetIO().DisplaySize;

  ImGui::SetNextWindowPos(ImVec2(0.0f, display.y - TASKBAR_HEIGHT));
  ImGui::SetNextWindowSize(ImVec2(display.x, TASKBAR_HEIGHT));
  ImGui::SetNextWindowBgAlpha(0.88f);

  constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoScrollbar;

  if (ImGui::Begin("##Taskbar", nullptr, flags)) {
    ImDrawList* draw = ImGui::GetWindowDrawList();

    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();

    draw->AddLine(pos, ImVec2(pos.x + size.x, pos.y), IM_COL32(255, 255, 255, 45));

    ImGui::SetCursorPos(ImVec2(12.0f, 9.0f));

    if (icon_button("files", file_explorer_icon, " File Explorer ", ImVec2(130.0f, 36.0f))) {
      app.is_file_explorer_open = !app.is_file_explorer_open;
    }

    ImGui::SameLine();

    if (icon_button("browser", browser_icon, " Browser ", ImVec2(90.0f, 36.0f))) {
      app.is_browser_open = !app.is_browser_open;
    }

    ImGui::SameLine();

    if (icon_button("task-manager", task_manager_icon, " Task Manager ", ImVec2(120.0f, 36.0f))) {
      app.is_task_manager_open = !app.is_task_manager_open;
    }

    const float power_w = 42.0f;
    const float clock_w = 80.0f;
    const float right_padding = 12.0f;

    ImGui::SetCursorPos(ImVec2(size.x - right_padding - power_w, 9.0f));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

    if (ImGui::Button("PWR", ImVec2(power_w, 36.0f))) {
      app.is_shutting_down = true;
    }

    ImGui::PopStyleColor(3);

    ImGui::SetCursorPos(ImVec2(size.x - right_padding - power_w - clock_w - 12.0f, 7.0f));

    ImGui::BeginGroup();

    ImGui::SetCursorPosX(size.x - right_padding - power_w - clock_w - 12.0f);
    ImGui::SetCursorPosY(11.0f);

    ImGui::TextUnformatted(clock::format_time().c_str());

    ImGui::SetCursorPosX(size.x - right_padding - power_w - clock_w - 12.0f);

    ImGui::TextDisabled("%s", clock::format_date().c_str());

    ImGui::EndGroup();
  }

  ImGui::End();
}

}  // namespace os_mockup::desktop

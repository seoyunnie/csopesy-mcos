#include "style.hpp"

#include <imgui.h>

namespace os_mockup::style {

void setup() {
  ImGui::StyleColorsDark();

  auto& style = ImGui::GetStyle();

  style.WindowRounding = 10.0f;
  style.ChildRounding = 8.0f;
  style.FrameRounding = 6.0f;
  style.PopupRounding = 8.0f;
  style.ScrollbarRounding = 8.0f;
  style.GrabRounding = 6.0f;

  style.WindowPadding = ImVec2(16.0f, 14.0f);
  style.FramePadding = ImVec2(10.0f, 7.0f);
  style.ItemSpacing = ImVec2(8.0f, 8.0f);

  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.075f, 0.095f, 0.14f, 0.97f);

  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.075f, 0.095f, 0.14f, 1.0f);

  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.14f, 0.21f, 1.0f);

  style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.23f, 0.34f, 1.0f);

  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.36f, 0.52f, 1.0f);

  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.43f, 0.72f, 1.0f);

  style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.25f, 0.38f, 1.0f);

  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.23f, 0.36f, 0.53f, 1.0f);

  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.67f, 1.0f, 1.0f);
}

}  // namespace os_mockup::style

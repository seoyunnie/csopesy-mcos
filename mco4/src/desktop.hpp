#ifndef DESKTOP_HPP_
#define DESKTOP_HPP_

#include "app_state.hpp"
#include "texture.hpp"

namespace os_mockup::desktop {

void draw_wallpaper(const texture::Texture& wallpaper);

void draw_taskbar(AppState& app, const texture::Texture& file_explorer_icon, const texture::Texture& browser_icon,
                  const texture::Texture& task_manager_icon);

}  // namespace os_mockup::desktop

#endif  // DESKTOP_HPP_

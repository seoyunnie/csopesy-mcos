#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

namespace os_mockup::texture {

struct Texture {
  unsigned int id = 0;
  int width = 0;
  int height = 0;

  [[nodiscard]] bool is_valid() const noexcept {
    return id != 0;
  }
};

Texture load(const char* path);

void destroy(Texture& texture);

}  // namespace os_mockup::texture

#endif  // TEXTURE_HPP_

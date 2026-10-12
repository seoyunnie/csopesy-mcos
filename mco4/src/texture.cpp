#include "texture.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace os_mockup::texture {

Texture load(const char* path) {
  int w = 0;
  int h = 0;

  stbi_uc* pixels = stbi_load(path, &w, &h, nullptr, STBI_rgb_alpha);

  if (pixels == nullptr) {
    std::fprintf(stderr, "Failed to load image \"%s\": %s\n", path, stbi_failure_reason());

    return {};
  }

  GLuint id = 0;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLint prev_alignment = 4;

  glGetIntegerv(GL_UNPACK_ALIGNMENT, &prev_alignment);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  glPixelStorei(GL_UNPACK_ALIGNMENT, prev_alignment);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(pixels);

  return {.id = id, .width = w, .height = h};
}

void destroy(Texture& texture) {
  if (texture.id != 0) {
    const GLuint id = texture.id;

    glDeleteTextures(1, &id);

    texture = {};
  }
}

}  // namespace os_mockup::texture

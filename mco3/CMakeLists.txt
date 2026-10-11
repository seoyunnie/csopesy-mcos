cmake_minimum_required(VERSION 3.15...4.4)

project(marquee-console
  VERSION 1.0.0
  DESCRIPTION "A program that displays and controls an animated marquee text."
  LANGUAGES CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Werror -Wextra -Wpedantic)
endif()

add_subdirectory(src)

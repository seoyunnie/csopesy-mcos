cmake_minimum_required(VERSION 3.15...4.4)

project(marquee-console
  VERSION 1.0.0
  DESCRIPTION "A program that displays and controls an animated marquee text."
  LANGUAGES CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory(src)

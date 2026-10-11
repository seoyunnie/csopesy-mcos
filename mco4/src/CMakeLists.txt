add_executable(os-mockup
  main.cpp
  app_state.hpp
  apps.cpp
  apps.hpp
  clock.cpp
  clock.hpp
  desktop.cpp
  desktop.hpp
  style.cpp
  style.hpp
  texture.cpp
  texture.hpp
)
target_link_libraries(os-mockup
  PRIVATE glfw
  PRIVATE OpenGL::GL
  PRIVATE imgui
)
target_include_directories(os-mockup
  PRIVATE "${PROJECT_SOURCE_DIR}/external"
)

if (MSVC)
  target_compile_options(os-mockup
    PRIVATE /W4
    PRIVATE /WX
  )
else()
  target_compile_options(os-mockup
    PRIVATE -Wall
    PRIVATE -Werror
    PRIVATE -Wextra
    PRIVATE -Wpedantic
  )
endif()

if(APPLE)
  target_compile_definitions(os-mockup
    PRIVATE GL_SILENCE_DEPRECATION
  )
endif()

add_custom_target(copy-assets ALL
  COMMAND "${CMAKE_COMMAND}" -E rm -rf "$<TARGET_FILE_DIR:os-mockup>/assets"
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${OS_MOCKUP_ASSETS_DIR}" "$<TARGET_FILE_DIR:os-mockup>/assets"
  COMMENT "Refreshing assets"
  VERBATIM
)

add_dependencies(os-mockup copy-assets)

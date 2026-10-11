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
)
target_link_libraries(os-mockup
  PRIVATE glfw
  PRIVATE OpenGL::GL
  PRIVATE imgui
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

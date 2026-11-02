add_executable(process-scheduler
  main.cpp
  cli.cpp
  cli.hpp
  config.cpp
  config.hpp
  process.hpp
  scheduler.cpp
  scheduler.hpp
)

add_custom_target(copy-config ALL
  COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/config.txt" "$<TARGET_FILE_DIR:process-scheduler>/config.txt"
  COMMENT "Copying config"
  VERBATIM
)

add_dependencies(process-scheduler copy-config)

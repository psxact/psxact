file(GLOB_RECURSE APP_SOURCES
  "./*.cpp"
  "./*.hpp")

add_executable(psxact ${APP_SOURCES})
target_link_libraries(psxact src)

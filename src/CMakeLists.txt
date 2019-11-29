file(GLOB_RECURSE SOURCES
  "./*.cpp"
  "./*.hpp")

add_executable(psxact ${SOURCES})
target_link_libraries(psxact ${SDL2_LIBRARY})

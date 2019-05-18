file(GLOB_RECURSE SOURCES
  "./*.cpp"
  "./*.hpp")

add_library(src STATIC ${SOURCES})
target_link_libraries(src ${SDL2_LIBRARY})

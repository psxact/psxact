cmake_minimum_required(VERSION 2.4)
project(psxact)

# Module configuration

set(SDL2_PATH "" CACHE INTERNAL "Fill in path to the SDL2 (Win32)")

set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/modules
  ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)

# Compiler flags

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(
  ${SDL2_INCLUDE_DIR}
  "src")

# Building

file(GLOB_RECURSE SOURCE_FILES
  "src/*.cpp"
  "src/*.hpp")

add_executable(psxact ${SOURCE_FILES})

target_link_libraries(psxact ${SDL2_LIBRARY})

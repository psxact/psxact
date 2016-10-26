cmake_minimum_required(VERSION 3.6)
project(psxact)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/cpu/cpu_core.cpp
    src/cpu/cpu_core.hpp
    src/cpu/decoder.hpp
    src/cpu/memory.cpp
    src/gpu/gpu_core.cpp
    src/gpu/gpu_core.hpp
    src/spu/spu_core.cpp
    src/spu/spu_core.hpp
    src/bus.cpp
    src/bus.hpp
    src/psxact.cpp
    src/utility.cpp
    src/utility.hpp)

add_executable(psxact ${SOURCE_FILES})
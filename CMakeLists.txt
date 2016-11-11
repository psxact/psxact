cmake_minimum_required(VERSION 3.6)
project(psxact)

find_package(SDL2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/cpu/cpu_code.cpp
    src/cpu/cpu_core.cpp
    src/cpu/cpu_core.hpp
    src/cpu/decoder.cpp
    src/cpu/disassembler.cpp
    src/dma/dma_core.cpp
    src/dma/dma_core.hpp
    src/gpu/gpu_core.cpp
    src/gpu/gpu_draw_gouraud.cpp
    src/gpu/gpu_draw_texture.cpp
    src/gpu/gpu_draw.cpp
    src/gpu/gpu_core.hpp
    src/spu/spu_core.cpp
    src/spu/spu_core.hpp
    src/timer/timer_core.cpp
    src/timer/timer_core.hpp
    src/bus.cpp
    src/bus.hpp
    src/psxact.cpp
    src/utility.cpp
    src/utility.hpp)

add_executable(psxact ${SOURCE_FILES})

target_link_libraries(psxact ${SDL2_LIBRARIES})

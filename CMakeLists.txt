cmake_minimum_required(VERSION 3.0)
project(psxact)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_MODULE_PATH}
)

set(SDL2_PATH "" CACHE INTERNAL "Fill in path to the SDL2 (Win32)")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    src/cdrom/cdrom_core.cpp
    src/cdrom/cdrom_core.hpp
    src/cpu/cpu_code.cpp
    src/cpu/cpu_core.cpp
    src/cpu/cpu_core.hpp
    src/cpu/decoder.cpp
    src/cpu/disassembler.cpp
    src/dma/dma_core.cpp
    src/dma/dma_core.hpp
    src/gpu/gpu_gp0.cpp
    src/gpu/gpu_gp1.cpp
    src/gpu/gpu_core.cpp
    src/gpu/gpu_core.hpp
    src/gpu/gpu_draw.cpp
    src/gpu/gpu_draw_gouraud.cpp
    src/gpu/gpu_draw_texture.cpp
    src/input/input.cpp
    src/input/input.hpp
    src/memory/vram.cpp
    src/memory/vram.hpp
    src/spu/spu_core.cpp
    src/spu/spu_core.hpp
    src/timer/timer_core.cpp
    src/timer/timer_core.hpp
    src/bus.cpp
    src/bus.hpp
    src/psxact.cpp
    src/renderer.cpp
    src/renderer.hpp
    src/utility.hpp)

add_executable(psxact ${SOURCE_FILES})

target_link_libraries(psxact ${SDL2_LIBRARY})

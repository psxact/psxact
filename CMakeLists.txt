cmake_minimum_required(VERSION 3.0)
project(psxact)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_MODULE_PATH})

set(SDL2_PATH "" CACHE INTERNAL "Fill in path to the SDL2 (Win32)")
find_package(SDL2 REQUIRED)
include_directories(
    ${SDL2_INCLUDE_DIR}
    "src"
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_FILES
        src/cdrom/cdrom.cpp
        src/cdrom/cdrom.hpp
        src/cdrom/cdrom_io.cpp
        src/counter/counter.cpp
        src/counter/counter.hpp
        src/cpu/cpu_code.cpp
        src/cpu/cpu_cop.hpp
        src/cpu/cpu_cop0.cpp
        src/cpu/cpu_cop0.hpp
        src/cpu/cpu_cop2_flag.cpp
        src/cpu/cpu_cop2_io.cpp
        src/cpu/cpu_cop2.cpp
        src/cpu/cpu_cop2.hpp
        src/cpu/cpu.cpp
        src/cpu/cpu.hpp
        src/cpu/decoder.cpp
        src/cpu/disassembler.cpp
        src/dma/dma.cpp
        src/dma/dma.hpp
        src/gpu/gpu_gp0.cpp
        src/gpu/gpu_gp1.cpp
        src/gpu/gpu.cpp
        src/gpu/gpu.hpp
        src/gpu/gpu_draw.cpp
        src/gpu/gpu_line.cpp
        src/gpu/gpu_poly.cpp
        src/gpu/gpu_rect.cpp
        src/input/input.cpp
        src/input/input.hpp
        src/mdec/mdec.cpp
        src/mdec/mdec.hpp
        src/memory/vram.cpp
        src/memory/vram.hpp
        src/spu/spu.cpp
        src/spu/spu.hpp
        src/fifo.hpp
        src/limits.hpp
        src/memory.hpp
        src/psxact.cpp
        src/sdl2.cpp
        src/sdl2.hpp
        src/console.cpp
        src/console.hpp
        src/utility.hpp)

add_executable(psxact ${SOURCE_FILES})

target_link_libraries(psxact ${SDL2_LIBRARY})

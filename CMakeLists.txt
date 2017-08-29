cmake_minimum_required(VERSION 3.0)
project(psxact)

set(CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_MODULE_PATH})

set(SDL2_PATH "" CACHE INTERNAL "Fill in path to the SDL2 (Win32)")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
        src/cdrom/cdrom_core.cpp
        src/cdrom/cdrom_core.hpp
        src/cdrom/cdrom_core_io.cpp
        src/cpu/cpu_code.cpp
        src/cpu/cpu_cop0.cpp
        src/cpu/cpu_cop0.hpp
        src/cpu/cpu_cop2_flag.cpp
        src/cpu/cpu_cop2_io.cpp
        src/cpu/cpu_cop2.cpp
        src/cpu/cpu_cop2.hpp
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
        src/gpu/gpu_line.cpp
        src/gpu/gpu_poly.cpp
        src/gpu/gpu_rect.cpp
        src/input/guest/digital_device.cpp
        src/input/guest/digital_device.hpp
        src/input/guest/input_device.hpp
        src/input/guest/null_device.cpp
        src/input/guest/null_device.hpp
        src/input/input_core.cpp
        src/input/input_core.hpp
        src/input/input_port.cpp
        src/input/input_port.hpp
        src/mdec/mdec_core.cpp
        src/mdec/mdec_core.hpp
        src/memory/vram.cpp
        src/memory/vram.hpp
        src/spu/spu_core.cpp
        src/spu/spu_core.hpp
        src/timer/timer_core.cpp
        src/timer/timer_core.hpp
        src/fifo.hpp
        src/limits.hpp
        src/memory.hpp
        src/psxact.cpp
        src/sdl2.cpp
        src/sdl2.hpp
        src/system_core.cpp
        src/system_core.hpp
        src/utility.hpp)

add_executable(psxact ${SOURCE_FILES})

target_link_libraries(psxact ${SDL2_LIBRARY})

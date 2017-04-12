include(FindPackageHandleStandardArgs)

set(SDL2_LIBRARY "" CACHE STRING "Fill in path to the SDL2.lib file")
set(SDL2_INCLUDE_DIR "" CACHE INTERNAL "Fill in path to the SDL2/include folder")

find_package_handle_standard_args(SDL2 REQUIRED_VARS SDL2_LIBRARY SDL2_INCLUDE_DIR)

include_directories(${SDL2_INCLUDE_DIR})

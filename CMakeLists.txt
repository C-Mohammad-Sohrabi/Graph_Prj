cmake_minimum_required(VERSION 3.10)
project(GraphTheoryProject C)

# ---------------------------------------------------------------------------
# Cross-platform build configuration
#
# This works identically on Windows (MSVC / MinGW), macOS (Xcode / Makefiles)
# and Linux (Makefiles / Ninja) via the standard CMake workflow:
#
#   cmake -B build -S .
#   cmake --build build
#
# The resulting executable is placed in build/bin/ (or build/bin/Debug on
# multi-config generators like Visual Studio).
# ---------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# Put the final executable in a predictable place on every platform.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB PROJECT_SOURCES ${CMAKE_SOURCE_DIR}/src/*.c)

add_executable(graph_program ${PROJECT_SOURCES})

target_include_directories(graph_program PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Warnings: -Wall -Wextra on GCC/Clang, /W4 on MSVC.
if(MSVC)
    target_compile_options(graph_program PRIVATE /W4)
else()
    target_compile_options(graph_program PRIVATE -Wall -Wextra)
    # math.h functions (sqrt, etc.) need libm on Linux/macOS with GCC/Clang;
    # MSVC links this in automatically, so this is guarded to non-Windows.
    if(NOT WIN32)
        target_link_libraries(graph_program m)
    endif()
endif()

message(STATUS "graph_program will be built to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Run it from the project root so build/dot_files and build/images land in the right place, e.g.:")
message(STATUS "  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/graph_program")

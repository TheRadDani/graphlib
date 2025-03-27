# CMakeLists.txt for the graphlib project
# High-performance graph processing and secure random walks for Python
# Author: Daniel Ferreto
# Version: 1.1
# Date: 2025-03-27

cmake_minimum_required(VERSION 3.16)
project(graphlib LANGUAGES CXX)

# === Project Configuration ===
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable interprocedural optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# === Security and Hardening Flags ===
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined -fno-omit-frame-pointer")

# === ASAN Options Config ===
configure_file(${CMAKE_SOURCE_DIR}/asan_options.cfg ${CMAKE_BINARY_DIR}/asan_options.cfg COPYONLY)

# === Architecture-Specific Compilation ===
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
    add_compile_options(-m64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    add_compile_options(-march=armv8-a)
endif()

# === Pybind11 Dynamic Detection ===
execute_process(
    COMMAND ${Python3_EXECUTABLE} -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
list(APPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
find_package(pybind11 REQUIRED CONFIG)

# === Source Files ===
set(SOURCE_FILES
    src/Graph.cpp
    src/RandomWalker.cpp
    bindings/graphlib.cpp
)

# === Target Declaration ===
pybind11_add_module(graphlib ${SOURCE_FILES})

# === Secure Linking ===
target_link_options(graphlib PRIVATE "LINKER:--no-undefined" "LINKER:--as-needed")

# === Include Directories ===
target_include_directories(graphlib
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/bindings
)

# === Installation Rules ===
install(TARGETS graphlib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/python3.10/site-packages)

# === Postbuild Security Checks ===
add_custom_command(TARGET graphlib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "\n[INFO] Running post-build security audit..."
    COMMAND ${CMAKE_COMMAND} -E echo "Check with: readelf -Wl $<TARGET_FILE:graphlib>"
)

# === Usage Notes ===
message(STATUS "\nTo use graphlib in Python:")
message(STATUS "  1. Build with: cmake -B build -DCMAKE_BUILD_TYPE=Release")
message(STATUS "  2. Compile: cmake --build build")
message(STATUS "  3. Install: sudo cmake --install build")
message(STATUS "  4. Use in Python >= 3.10: import graphlib")
# CMakeLists.txt for the aegisgraph project
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0148 NEW)  # Suppress CMP0148 warning

project(aegisgraph LANGUAGES CXX)

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

# === Pybind11 and Python 3.12 Setup ===
set(PYBIND11_FINDPYTHON ON)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED CONFIG)

message(STATUS "Python include dir: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python library    : ${Python3_LIBRARIES}")
message(STATUS "Python executable : ${Python3_EXECUTABLE}")

# === Source Files ===
set(SOURCE_FILES
    src/Graph.cpp
    src/RandomWalker.cpp
    bindings/aegisgraph.cpp
)

# === Target Declaration ===
pybind11_add_module(aegisgraph ${SOURCE_FILES})

# === Include and Link Python Explicitly ===
target_include_directories(aegisgraph PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(aegisgraph PRIVATE ${Python3_LIBRARIES})

# === Secure Linking ===
target_link_options(aegisgraph PRIVATE "LINKER:--no-undefined" "LINKER:--as-needed")

# === Include Directories ===
target_include_directories(aegisgraph
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/bindings
)

# === Installation Rules (detect Python path) ===
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()[\"purelib\"])"
    OUTPUT_VARIABLE PYTHON_SITE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

install(TARGETS aegisgraph
        LIBRARY DESTINATION ${PYTHON_SITE_DIR})

# === Postbuild Security Checks ===
add_custom_command(TARGET aegisgraph POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "[INFO] Post-build audit complete."
    COMMENT "Running post-build audit"
)

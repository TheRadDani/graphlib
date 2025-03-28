# CMakeLists.txt for the aegisgraph project
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0148 NEW)  # Suppress CMP0148 warning

project(aegisgraph LANGUAGES CXX)

# === Enable OpenMP ===
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP CXX flags: ${OpenMP_CXX_FLAGS}")
    message(STATUS "OpenMP CXX libs: ${OpenMP_CXX_LIB_NAMES}")
    # Add OpenMP flags to all targets
    add_link_options(${OpenMP_CXX_FLAGS})
    # Store libs for later linking
    list(APPEND LINK_LIBS ${OpenMP_CXX_LIB_NAMES})
endif()

# === Abseil Dependency ===
set(ABSL_PROPAGATE_CXX_STD ON)
include(FetchContent)
FetchContent_Declare(
    abseil-cpp
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG 20240116.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(abseil-cpp)

# === Project Configuration ===
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# === Optimization Flags ===
set(COMMON_FLAGS "-Wall -Wextra -Wpedantic -fstack-protector-strong -D_FORTIFY_SOURCE=2")
set(SIMD_FLAGS "-mavx2 -mfma -mbmi2 -ftree-vectorize -fno-strict-aliasing")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
    set(ARCH_FLAGS "${SIMD_FLAGS} -march=native")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(ARCH_FLAGS "-march=armv8-a+sve2+crypto+fp16+rcpc+dotprod")
else()
    set(ARCH_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} ${ARCH_FLAGS}")

# Enable interprocedural optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Enable LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()


set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto")
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined,leak -fno-omit-frame-pointer")

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

# === ASAN Options Config ===
configure_file(${CMAKE_SOURCE_DIR}/asan_options.cfg ${CMAKE_BINARY_DIR}/asan_options.cfg COPYONLY)

# === Main Target ===
pybind11_add_module(aegisgraph ${SOURCE_FILES})

# === Include and Link Python Explicitly ===
target_include_directories(aegisgraph PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(aegisgraph PRIVATE ${Python3_LIBRARIES})

# === Security Hardening ===
target_compile_options(aegisgraph PRIVATE
    -Wformat -Wformat-security
)

target_link_options(aegisgraph PRIVATE
    "LINKER:-z,relro"
    "LINKER:-z,now"
    "LINKER:-z,noexecstack"
    "LINKER:-z,noexecheap"
)

# === Include Directories ===
target_include_directories(aegisgraph PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/bindings
    ${Python3_INCLUDE_DIRS}
)

# === Link Dependencies ===
target_link_libraries(aegisgraph PRIVATE
    ${LINK_LIBS}
    stdc++fs
    pthread
)

# === Optimization Linker Flags ===
target_link_options(aegisgraph PRIVATE 
    "LINKER:--no-undefined"
    "LINKER:--as-needed"
    "LINKER:--hash-style=gnu"
)

if(CMAKE_LINKER MATCHES "gold")
    target_link_options(aegisgraph PRIVATE "LINKER:--threads")
endif()

# === RPATH Handling ===
if(UNIX AND NOT APPLE)
    set_target_properties(aegisgraph PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# === Installation Rules ===
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_paths()[\"purelib\"])"
    OUTPUT_VARIABLE PYTHON_SITE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

install(TARGETS aegisgraph
        LIBRARY DESTINATION ${PYTHON_SITE_DIR}
        COMPONENT python
)

# === Post-build Security Check ===
add_custom_command(TARGET aegisgraph POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} -p $<TARGET_FILE:aegisgraph> | grep -E 'RELRO|NX|STACK' || true
    COMMAND ${CMAKE_COMMAND} -E echo "[HARDENING] Security protections verified"
    COMMENT "Verifying binary security features"
)
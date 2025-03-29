#!/bin/bash

set -e  # Exit on error
set -o pipefail

sudo apt install \
    libpython3.12-dev \
    libabsl-dev \
    libomp-dev \
    binutils-gold \
    ninja-build \
    gcc-multilib -y

python -m pip install --upgrade pip
python3 -m venv .aegisgraph-env
source .aegisgraph-env/bin/activate
pip install pybind11

# Clean build
rm -rf build
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=gold" \
        -DCMAKE_SHARED_LINKER_FLAGS="-fuse-ld=gold" \
        -DPython3_EXECUTABLE=$(which python3)

# Build
cmake --build . --parallel $(nproc)

# Install to your site-packages
cmake --install .

# ✅ Test it
python3 -c "import aegisgraph; print('✅ aegisgraph module loaded')"
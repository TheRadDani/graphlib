sudo apt install build-essential clang cmake pybind11-dev -y

python3 -m ensurepip --upgrade

pip install --upgrade pip
pip install pybind11

mkdir -p build && cd build

# Basic secure build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Enable sanitizers (recommended for dev testing)
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo \
         -DENABLE_ASAN=ON \
         -DENABLE_UBSAN=ON

cmake --build . --config Release

cmake --install .

export PYTHONPATH=$PWD:$PYTHONPATH

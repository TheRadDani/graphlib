FROM python:3.12-slim

# System dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3-dev \
    python3-pip \
    libpython3.12-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Python packages
RUN pip install --upgrade pip
RUN pip install pybind11 pytest

# Set workdir and copy everything
WORKDIR /graphlib
COPY . .

# Configure, build, and install the library
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --parallel && \
    cmake --install .

# Run tests
CMD ["pytest", "tests"]
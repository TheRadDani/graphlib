# Build stage
FROM python:3.12-slim-bookworm AS builder

# Install build dependencies
RUN apt-get update && \
    apt-get install -y wget gnupg && \
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    echo "deb http://apt.llvm.org/bookworm/ llvm-toolchain-bookworm-16 main" >> /etc/apt/sources.list && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ninja-build \
    libabsl-dev \
    libpython3-dev \
    pkg-config \
    binutils-gold \
    && rm -rf /var/lib/apt/lists/*

# Install modern CMake (3.28+)
RUN wget -q https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-linux-x86_64.sh && \
    mkdir -p /opt/cmake && \
    sh cmake-3.28.3-linux-x86_64.sh --prefix=/opt/cmake --skip-license && \
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake

WORKDIR /app
COPY . .

RUN rm -rf build/ _skbuild/

# Configure build to match local environment structure
ENV SKBUILD_BUILD_OPTIONS="-DCMAKE_INSTALL_PREFIX=/opt/venv/lib/python3.12/site-packages"

# Create wheel package
RUN pip install --upgrade pip setuptools wheel && \
    pip install scikit-build-core pybind11

# Build with verbose logging and gold linker
RUN pip wheel --wheel-dir=/app/wheels -v . \
    --config-settings="cmake.args=-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=gold -DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=gold" \
    --timeout 60 \
    --retries 3
    
# Runtime stage
FROM python:3.12-slim-bookworm

# Install runtime dependencies with cleanup in single layer
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libgomp1=12.2.0-14 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Create and actiavate virtual environment
RUN python -m venv /opt/venv --copies
ENV PATH="/opt/venv/bin:$PATH" \
    PYTHONPATH="/opt/venv/lib/python3.12/site-packages"

# Install wheel in virtual environment
COPY --from=builder /app/wheels /tmp/wheels
RUN /opt/venv/bin/python -m pip install --no-cache-dir --force-reinstall /tmp/wheels/aegisgraph-*.whl && \
    /opt/venv/bin/python -c "import aegisgraph" && \
    rm -rf /tmp/wheels

# Create application user with secure umask
RUN useradd -r -u 1000 -d /app -s /bin/false appuser && \
    mkdir -p /app && \
    chown -R appuser:appuser /app

# Security hardening
ENV PYTHONUNBUFFERED="1" \
    PYTHONDONTWRITEBYTECODE="1" \
    PYTHONFAULTHANDLER="1" \
    OMP_NUM_THREADS="1"

USER appuser
WORKDIR /app

# Health check
HEALTHCHECK --interval=30s --timeout=10s \
    CMD python -c "import aegisgraph" || exit 1

# Metadata
LABEL maintainer="Daniel Ferreto <ldanielfch@gmail.com>" \
      org.opencontainers.image.source="https://github.com/TheRadDani/aegisgraph" \
      org.opencontainers.image.licenses="MIT"

# Secure entrypoint
ENTRYPOINT ["python", "-I"]

CMD ["-c", "import aegisgraph; print(f'✅ Module loaded successfully - v{aegisgraph.__version__}')"]
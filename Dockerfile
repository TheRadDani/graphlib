# Build stage
FROM python:3.12-slim-bookworm as builder

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    libomp-dev \
    libpython3.12-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Install with build cache and without tests
RUN --mount=type=cache,target=/root/.cache/pip \
    --mount=type=cache,target=/root/.cache/scikit-build \
    pip install --user --no-cache-dir .

# Runtime stage
FROM python:3.12-slim-bookworm

# Install runtime dependencies with cleanup in single layer
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libgomp1=12.2.0-14 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    /var/log/alternatives.log \
    /var/log/apt \
    /var/log/dpkg.log

# Create application user with secure umask
RUN useradd -r -u 1000 -d /app -s /bin/false appuser && \
    mkdir -p /app && \
    chown -R appuser:appuser /app

# Copy installed package
COPY --from=builder --chown=appuser:appuser /root/.local /home/appuser/.local

# Security hardening
ENV PATH="/app/.local/bin:${PATH}" \
    PYTHONUNBUFFERED="1" \
    PYTHONDONTWRITEBYTECODE="1" \
    OMP_NUM_THREADS="1" \
    PYTHONFAULTHANDLER="1" \
    PIP_NO_PYTHON_VERSION_WARNING="1"

USER appuser
WORKDIR /app

# Health check
HEALTHCHECK --interval=30s --timeout=10s \
    CMD python -c "import aegisgraph" || exit 1

# Metadata
LABEL maintainer="Daniel Ferreto <ldanielfch@gmail.com>" \
      org.opencontainers.image.source="https://github.com/TheRadDani/aegisgraph" \
      org.opencontainers.image.licenses="MIT"

CMD ["-c", "import aegisgraph; print(f'✅ Module loaded successfully - v{aegisgraph.__version__}')"]

# Secure entrypoint
ENTRYPOINT ["python", "-I", "-X", "frozen_modules=off"]
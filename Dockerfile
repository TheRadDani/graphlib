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
    pip install --user --no-cache-dir .

# Runtime stage
FROM python:3.12-slim-bookworm

# Install runtime dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends libgomp1 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Create application user
RUN useradd -m -u 1000 appuser && \
    mkdir -p /app && \
    chown -R appuser:appuser /app

# Copy installed package
COPY --from=builder --chown=appuser:appuser /root/.local /home/appuser/.local

# Environment configuration
ENV PATH="/home/appuser/.local/bin:${PATH}" \
    PYTHONUNBUFFERED="1" \
    OMP_NUM_THREADS="1" \
    PYTHONPATH="/app"

USER appuser
WORKDIR /app

# Health check
HEALTHCHECK --interval=30s --timeout=10s \
    CMD python -c "import aegisgraph" || exit 1

# Default entrypoint
ENTRYPOINT ["python"]
CMD ["python", "-c", "import aegisgraph; print('✅ Module loaded successfully')"]
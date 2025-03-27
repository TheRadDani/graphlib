FROM python:3.12-slim AS builder

# Install OS-level dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    python3-dev \
    libpython3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set environment variables
ENV VIRTUAL_ENV=/opt/venv
RUN python3 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

# Copy project files
WORKDIR /app
COPY . /app

# Upgrade pip + install build dependencies
RUN pip install --upgrade pip
RUN pip install scikit-build-core pybind11 pytest

# Install AegisGraph in editable mode
RUN pip install -e .

# Run tests to ensure installation works
RUN pytest --maxfail=1 --disable-warnings -q tests/

# Final image (optional, minimal)
FROM python:3.12-slim AS final

# Set virtualenv path
ENV VIRTUAL_ENV=/opt/venv
COPY --from=builder $VIRTUAL_ENV $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

# Copy source code and built library
WORKDIR /app
COPY . /app

# Entry point for testing the module
CMD ["python3", "-c", "import aegisgraph; print('✅ aegisgraph loaded')"]
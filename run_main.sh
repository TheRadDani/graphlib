export ASAN_OPTIONS=$(cat ../asan_options.cfg | paste -sd:)
PYTHONMALLOC=malloc python3 main.py

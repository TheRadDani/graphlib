export ASAN_OPTIONS=$(cat asan_options.cfg | paste -s -d:)
python3 -c "import graphlib"

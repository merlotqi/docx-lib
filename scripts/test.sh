#!/usr/bin/env bash
# Build (if needed) and run the docx-lib test suite via CTest.
#
# Usage: scripts/test.sh [ctest args...]
#   e.g. scripts/test.sh -V
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."

if [ ! -f build/CMakeCache.txt ]; then
  ./scripts/build.sh Debug
elif [ ! -f build/CTestTestfile.cmake ] && [ ! -f build/tests/CTestTestfile.cmake ]; then
  # Cache exists but was configured with tests disabled; reconfigure.
  cmake -S . -B build -DDOCX_BUILD_TESTS=ON
  cmake --build build --parallel
else
  cmake --build build --parallel
fi

ctest --test-dir build --output-on-failure "$@"

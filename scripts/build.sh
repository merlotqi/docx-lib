#!/usr/bin/env bash
# Configure and build docx-lib.
#
# Usage: scripts/build.sh [BuildType] [extra cmake args...]
#   e.g. scripts/build.sh Release -DDOCX_WARNINGS_AS_ERRORS=ON
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."

build_type="${1:-Debug}"
if [ "$#" -gt 0 ]; then shift; fi

echo "Configuring (${build_type})..."
cmake -S . -B build -DCMAKE_BUILD_TYPE="${build_type}" "$@"

echo "Building..."
cmake --build build --parallel

echo "Build finished: build/"

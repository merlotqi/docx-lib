#!/usr/bin/env bash
# Format all C++ sources in place using the repository .clang-format.
# (Extra arguments are appended to the clang-format invocation.)
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."

if ! command -v clang-format >/dev/null 2>&1; then
  echo "error: clang-format not found on PATH" >&2
  exit 1
fi

mapfile -t files < <(find include src tests examples \
  -type f \( -name '*.hpp' -o -name '*.hh' -o -name '*.h' \
             -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' \) \
  2>/dev/null | sort)

if [ "${#files[@]}" -eq 0 ]; then
  echo "No C++ files found."
  exit 0
fi

clang-format --style=file -i "$@" "${files[@]}"
echo "Formatted ${#files[@]} file(s)."

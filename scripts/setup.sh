#!/usr/bin/env bash
# Verify development prerequisites and install the pre-commit git hooks.
set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/.."

fail=0

require() {
  local cmd="$1" hint="$2"
  if command -v "${cmd}" >/dev/null 2>&1; then
    printf '  [ok] %-14s %s\n' "${cmd}" "$("${cmd}" --version 2>/dev/null | head -n1 || true)"
  else
    printf '  [MISSING] %-14s %s\n' "${cmd}" "${hint}"
    fail=1
  fi
}

echo "Checking docx-lib development prerequisites..."
require cmake "install CMake >= 3.16 (apt install cmake)"
require g++ "install a C++20 compiler (g++ >= 12 or clang++ >= 14)"
require python3 "install Python >= 3.8 (needed by pre-commit)"
require pre-commit "pip install pre-commit (or: pipx install pre-commit)"
require clang-format "install clang-format 18 (apt install clang-format)"

if [ "${fail}" -ne 0 ]; then
  echo
  echo "Some prerequisites are missing -- see hints above." >&2
  exit 1
fi

echo
echo "Installing git hooks..."
pre-commit install
echo "Done. Hooks will now run on 'git commit'."
echo "Manual run:  pre-commit run --all-files"

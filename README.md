# docx-lib

A C++ library for reading and writing `.docx` (Office Open XML / OOXML)
documents.

> **Status: preliminary design.** The public headers and project scaffolding
> exist, but parsing/writing is not implemented yet. The goal is a
> fully-fidelity `.docx` parser (OPC package layer → WordprocessingML document
> model → styles/numbering/tables), not just a thin read/write wrapper.

## Features (planned)

- **OPC package layer** — ZIP container, content types, relationships
  ([ECMA-376 Part 2](https://ecma-international.org/publications-and-standards/standards/ecma-376/))
- **Document model** — paragraphs, runs, text, tables, visitors
- **Formatting** — run/paragraph properties, style inheritance, themes
- **Lossless round-trip** — preserve parts and properties the library doesn't
  interpret yet
- **Tolerant parsing** — real-world files frequently violate the schemas

## Project layout

```
docx-lib/
├── CMakeLists.txt          # build definition (library, tests, install)
├── include/docx/           # public headers
│   ├── docx.hpp            # umbrella header
│   ├── document.hpp        # Document entry point
│   ├── common/             # shared enums, colors, units
│   ├── model/              # Node/Paragraph/Run/Table document tree
│   ├── opc/                # package/part (OPC) layer
│   └── style/              # property structs & style types
├── src/                    # library implementation
├── tests/                  # CTest-based test suite
├── docs/references/        # OOXML reference notes (start here: docs/references/README.md)
├── scripts/                # setup / build / test / format helpers
└── .pre-commit-config.yaml # git hook checks
```

## Requirements

- CMake ≥ 3.16
- A C++20 compiler (GCC ≥ 12, Clang ≥ 14, MSVC ≥ 19.30)

Development extras (installed via `./scripts/setup.sh`):

- Python ≥ 3.8 + [`pre-commit`](https://pre-commit.com)
- `clang-format` 18 (matches CI)

## Building

```bash
./scripts/build.sh            # Debug build into ./build
./scripts/build.sh Release    # Release build
```

or manually:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

CMake options:

| Option | Default | Meaning |
|--------|---------|---------|
| `DOCX_BUILD_TESTS` | `ON` (standalone) | build the test suite |
| `DOCX_INSTALL` | `ON` (standalone) | generate install rules |
| `DOCX_WARNINGS_AS_ERRORS` | `OFF` | `-Werror` / `/WX` |

## Testing

```bash
./scripts/test.sh             # builds if needed, then runs ctest
# or
ctest --test-dir build --output-on-failure
```

## Using the library

```cpp
#include <docx/docx.hpp>

// planned API — not yet implemented:
// docx::Document doc;
// doc.open("example.docx");
```

Install + consume from CMake:

```bash
cmake --install build --prefix /usr/local
```

```cmake
find_package(docx REQUIRED)
target_link_libraries(app PRIVATE docx::docx)
```

## Development

```bash
./scripts/setup.sh            # checks tools, installs git hooks
pre-commit run --all-files    # lint/format checks on demand
./scripts/format.sh           # format C++ sources in place
```

Formatting follows [`.clang-format`](.clang-format) (Google style, 120 columns).
CI (GitHub Actions) runs pre-commit plus Debug/Release builds with warnings as
errors.

## Documentation

OOXML specification notes live in
[`docs/references/`](docs/references/README.md) — start with the
[index there](docs/references/README.md) for ECMA-376 links, spec reading
order, and how the references map onto this codebase.

## License

GPL-3.0-or-later — see [COPYING](COPYING).

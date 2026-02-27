# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with
code in this repository.

## Project Overview

wordNebula is a terminal-based word processor written in C++ that aims to
provide a distraction-free writing environment. The project follows the
Model-View-Presenter (MVP) architectural pattern with a focus on professional
development practices, comprehensive testing, and code quality.

## Build System

This project uses CMake (3.16+) with C++20 standard and modern build tools.

### Compiler Configuration

This project defaults to **Clang 16** for compilation:

- Better diagnostics and error messages
- Faster incremental builds
- Native integration with clang-tidy
- Superior sanitizer support

The DevContainer environment is pre-configured to use Clang. To explicitly
specify a compiler:

```bash
# Use Clang (default)
cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang

# Use GCC (alternative)
cmake -B build -DCMAKE_CXX_COMPILER=g++-12 -DCMAKE_C_COMPILER=gcc-12
```

### Quick Start

```bash
# Configure with Ninja (recommended)
cmake -B build -G Ninja

# Build
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Run the application
./build/bin/wordNebula
```

### Build Options

The CMakeLists.txt provides several configuration options:

```bash
# Enable/disable options
cmake -B build \
  -DBUILD_TESTING=ON \
  -DENABLE_COVERAGE=ON \
  -DENABLE_SANITIZERS=ON \
  -DENABLE_CLANG_TIDY=ON \
  -DENABLE_CPPCHECK=ON \
  -DBUILD_DOCUMENTATION=ON

# Build types: Debug (default), Release, RelWithDebInfo, MinSizeRel
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

**Available Build Options:**

- `BUILD_TESTING` (default: ON) - Build unit tests
- `ENABLE_COVERAGE` (default: OFF) - Enable code coverage reporting
- `ENABLE_SANITIZERS` (default: OFF) - Enable AddressSanitizer and UBSan
- `ENABLE_CLANG_TIDY` (default: OFF) - Run clang-tidy during build
- `ENABLE_CPPCHECK` (default: OFF) - Run cppcheck during build
- `BUILD_DOCUMENTATION` (default: OFF) - Generate Doxygen documentation

### Code Formatting

Code style follows LLVM with 4-space indentation and 200-character column limit
(see [.clang-format](.clang-format)).

Formatting is handled automatically by pre-commit hooks. To manually format all
source files:

```bash
# Format all source files using CMake target
cmake --build build --target format

# Or run pre-commit on all files
pre-commit run --all-files
```

## Testing

Tests use Google Test framework and are located in [tests/](tests/).

```bash
# Build and run all tests
cmake --build build
ctest --test-dir build --output-on-failure

# Run specific test
./build/test_textbuffer
./build/test_gapbuffer

# Generate coverage report (requires -DENABLE_COVERAGE=ON)
cmake --build build --target coverage
# View report at build/coverage/index.html
```

**Test Organization:**

- `test_TextBuffer.cpp` - Tests for TextBuffer implementation
- `test_GapBuffer.cpp` - Tests for GapBuffer implementation

## Code Quality Tools

### Static Analysis

```bash
# Run clang-tidy
cmake -B build -DENABLE_CLANG_TIDY=ON
cmake --build build

# Run cppcheck
cppcheck --enable=all --suppress=missingIncludeSystem src/ include/
```

Configuration: [.clang-tidy](.clang-tidy)

### Runtime Analysis

```bash
# Build with sanitizers
cmake -B build -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/bin/wordNebula

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./build/bin/wordNebula
```

### Pre-commit Hooks

```bash
# Install pre-commit hooks (first time setup)
pre-commit install

# Run manually on all files
pre-commit run --all-files
```

Configuration: [.pre-commit-config.yaml](.pre-commit-config.yaml)

## Documentation

### API Documentation (Doxygen)

Generate API documentation using Doxygen:

```bash
cmake -B build -DBUILD_DOCUMENTATION=ON
cmake --build build --target docs
# Open build/docs/html/index.html
```

### Design Documentation

Design and architectural discussions should be documented in `docs/designs/`:

- **Purpose**: Capture design decisions, architectural rationale, and trade-off
  analysis
- **Format**: Markdown files with clear headings and diagrams (ASCII art,
  Mermaid)
- **Naming**: Use descriptive names (e.g., `PHASE1_ARCHITECTURE.md`,
  `DESIGN_MVP_WORD_PROCESSOR.md`)
- **Content**: Focus on the "why" behind decisions, not just the "what"

**Current design documents:**

- [docs/designs/PHASE1_ARCHITECTURE.md](docs/designs/PHASE1_ARCHITECTURE.md) -
  Phase 1 implementation guide with diagrams
- [docs/designs/DESIGN_MVP_WORD_PROCESSOR.md](docs/designs/DESIGN_MVP_WORD_PROCESSOR.md)
  \- Design decisions and rationale
- [docs/designs/BRANCHING_STRATEGY.md](docs/designs/BRANCHING_STRATEGY.md) - Git
  workflow and branch protection

**When working with Claude Code**: Save important design conversations and
architectural discussions to `docs/designs/` for future reference and portfolio
documentation.

## Architecture

The codebase follows the **Model-View-Presenter (MVP)** pattern:

### Model ([src/Model/](src/Model/), [include/Model/](include/Model/))

Manages text buffer and edit tracking.

- **`IBuffer`** - Interface defining buffer operations (insert, delete, cursor
  movement)
- **`TextBuffer`** - Simple string-based buffer implementation
- **`GapBuffer`** - Gap buffer implementation for efficient editing (in
  development)

### View ([src/View/](src/View/))

Terminal UI using ncurses, handles user input.

- **`WNebulaView`** - Renders UI and captures user input

### Presenter ([src/Presenter/](src/Presenter/))

Coordinates Model and View, handles business logic.

- **`WNebulaPresenter`** - Main coordination logic, file operations, logging
  setup

All components are under the `wnebula` namespace.

### Main Entry Point

[src/WordNebula.cpp](src/WordNebula.cpp) sets up:

1. Logging directory (`logs/`)
1. spdlog logger instance (`logs/WordNebula.log`)
1. MVP components (Presenter, Model, View)
1. Component wiring and application run loop

## Dependencies

### Core Libraries

- **FTXUI** - Modern C++ terminal UI library (functional/component-based)
- **ncurses** - Traditional terminal UI library (fallback/alternative)
- **spdlog** - Logging library

### Development Tools

- **Google Test** - Unit testing framework
- **clang-format** - Code formatting
- **clang-tidy** - Static analysis
- **cppcheck** - Additional static analysis
- **Doxygen** - Documentation generation
- **lcov/gcov** - Code coverage
- **Valgrind** - Memory analysis

### Dependency Management

Dependencies can be managed through:

1. **System packages** (apt-get) - Configured in
   `.devcontainer/postCreateInstall.sh`
1. **vcpkg** - C++ package manager (configured in `vcpkg.json`)

## Platform Support

### Target Platforms

wordNebula is designed to work on **Linux and macOS** systems with modern
terminal emulators. All dependencies are cross-platform and well-tested on
Unix-like systems.

| Platform    | Support Status    | Notes                                          |
| ----------- | ----------------- | ---------------------------------------------- |
| **Linux**   | ✅ Primary Target | Fully supported, developed and tested on Linux |
| **macOS**   | ✅ Primary Target | Fully supported via vcpkg and native libraries |
| **Windows** | 🔮 Future         | Possible with FTXUI-only build (no ncurses)    |

### Platform-Specific Details

#### Linux (Primary Development Platform)

- **Terminal Emulators**: GNOME Terminal, Konsole, xterm, Alacritty, kitty, etc.
- **Package Management**: vcpkg handles all dependencies
- **Compiler**: Clang 16+ (default), GCC 12+ (alternative)
- **Build System**: CMake 3.16+ with Ninja
- **DevContainer**: Fully configured for Linux development

#### macOS (Fully Supported)

- **Terminal Emulators**: Terminal.app, iTerm2, Alacritty, kitty
- **Package Management**: vcpkg (standalone or via Homebrew)
- **Compiler**: Clang (system default via Xcode Command Line Tools)
- **Native Libraries**: ncurses comes pre-installed on macOS
- **Build Process**: Identical to Linux (CMake + vcpkg)

**Building on macOS:**

```bash
# Install vcpkg (if not already installed)
git clone https://github.com/Microsoft/vcpkg.git ~/vcpkg
~/vcpkg/bootstrap-vcpkg.sh

# Clone and build wordNebula
git clone <repository-url> wordNebula
cd wordNebula

# Configure with vcpkg toolchain
cmake -B build -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build

# Run
./build/bin/wordNebula
```

#### Cross-Platform Compatibility

All core dependencies are cross-platform:

- ✅ **FTXUI** - Works on Linux, macOS, Windows
- ✅ **spdlog** - Pure C++, works everywhere
- ✅ **Google Test** - Cross-platform testing framework
- ✅ **CMake + vcpkg** - Industry-standard cross-platform tools
- ⚠️ **ncurses** - Linux/macOS native, requires PDCurses on Windows

### Future Windows Support

If Windows support is desired in the future:

1. **Option A**: Use FTXUI exclusively (drop ncurses dependency)

   - FTXUI works natively on Windows
   - Simplifies codebase (single UI library)
   - Maintains feature parity across all platforms

1. **Option B**: Conditional compilation with PDCurses

   - Keep ncurses for Linux/macOS
   - Use PDCurses on Windows
   - More complex build configuration

**Recommendation**: Use FTXUI for new View implementation, which provides
automatic Windows support if needed.

### Terminal Feature Requirements

wordNebula requires a terminal with:

- **256-color support** (or better)
- **Unicode/UTF-8** support
- **Cursor positioning** and control sequences
- **Keyboard input** with modifier keys (Ctrl, Alt)
- **Terminal resize detection**

All modern terminal emulators on Linux and macOS support these features.

## Development Environment

The project uses DevContainer for consistent development across machines.

### VS Code Configuration

- **Launch Configurations** ([.vscode/launch.json](.vscode/launch.json)):

  - Debug wordNebula
  - Debug with sanitizers
  - Debug tests
  - Valgrind memory check

- **Tasks** ([.vscode/tasks.json](.vscode/tasks.json)):

  - CMake configure
  - CMake build
  - CMake clean
  - clang-format

### DevContainer Setup

After opening in DevContainer, the environment includes:

- C++ build tools (gcc, clang, cmake, ninja)
- All development dependencies
- VS Code extensions for C++ development
- Configured git hooks

## CI/CD

GitHub Actions workflow ([.github/workflows/ci.yml](.github/workflows/ci.yml))
runs on every push:

- Build and test on Ubuntu
- Code coverage reporting
- Static analysis (clang-tidy, cppcheck)
- Code formatting checks
- Sanitizer builds

## Coding Guidelines

1. **C++ Standard**: Use C++20 features appropriately
1. **Formatting**: Handled automatically by pre-commit hooks on commit
1. **Testing**: Write unit tests for new functionality
1. **Naming Conventions** (enforced by clang-tidy):
   - Classes/Structs: `CamelCase`
   - Functions/Methods: `camelBack`
   - Variables: `camelBack`
   - Constants: `UPPER_CASE`
   - Namespaces: `lower_case`
1. **Warnings**: Fix all compiler warnings (treat warnings as errors)
1. **Static Analysis**: Address clang-tidy and cppcheck findings
1. **Memory Safety**: Run with sanitizers and Valgrind before PR

## Development Workflow

### First-Time Setup

```bash
# Install pre-commit hooks
pre-commit install

# Configure build
cmake -B build -G Ninja

# Build and test
cmake --build build
ctest --test-dir build --output-on-failure
```

### Runtime Logs

Application logs are written to `logs/WordNebula.log` using spdlog.

## Current Development Focus

GapBuffer is fully implemented as the primary buffer mechanism (see
[include/Model/GapBuffer.hpp](include/Model/GapBuffer.hpp) and
[src/Model/GapBuffer.cpp](src/Model/GapBuffer.cpp)). All IBuffer interface
methods are implemented and tested.

**Test Suite**: Comprehensive tests in
[tests/test_GapBuffer.cpp](tests/test_GapBuffer.cpp)

**Future Work**: Piece table implementation for advanced undo/redo support

**Future Work**: Piece table implementation for advanced undo/redo support

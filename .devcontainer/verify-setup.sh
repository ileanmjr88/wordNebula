#!/bin/bash
# Verification script for wordNebula development container

echo "========================================"
echo "🔍 WordNebula Dev Container Verification"
echo "========================================"
echo ""

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

check_command() {
    local cmd=$1
    local expected_version=$2
    local friendly_name=$3

    if command -v $cmd &> /dev/null; then
        version=$($cmd --version 2>&1 | head -n1)
        echo -e "${GREEN}✓${NC} $friendly_name: $version"
        if [ ! -z "$expected_version" ]; then
            if echo "$version" | grep -q "$expected_version"; then
                echo -e "  ${GREEN}  Version matches expectation${NC}"
            else
                echo -e "  ${YELLOW}  Expected version: $expected_version${NC}"
            fi
        fi
    else
        echo -e "${RED}✗${NC} $friendly_name: NOT FOUND"
        return 1
    fi
}

check_library() {
    local lib=$1
    local friendly_name=$2

    if pkg-config --exists $lib 2>/dev/null; then
        version=$(pkg-config --modversion $lib 2>/dev/null)
        echo -e "${GREEN}✓${NC} $friendly_name: $version"
    else
        echo -e "${RED}✗${NC} $friendly_name: NOT FOUND"
        return 1
    fi
}

echo "📦 Build Tools:"
check_command cmake "3" "CMake"
check_command ninja "" "Ninja"
check_command ccache "" "ccache"

echo ""
echo "🔧 Compilers:"
check_command gcc-12 "12" "GCC"
check_command g++-12 "12" "G++"
check_command clang-16 "16" "Clang"
check_command clang++-16 "16" "Clang++"

echo ""
echo "🐛 Debugging Tools:"
check_command gdb "" "GDB"
check_command lldb-16 "" "LLDB"
check_command valgrind "" "Valgrind"

echo ""
echo "🔍 Static Analysis:"
check_command clang-format-16 "" "clang-format"
check_command clang-tidy-16 "" "clang-tidy"
check_command cppcheck "" "cppcheck"

echo ""
echo "📚 C++ Libraries:"
check_library ncurses "ncurses"
check_library spdlog "spdlog"

echo ""
echo "🧪 Testing:"
if [ -f "/usr/src/googletest/CMakeLists.txt" ]; then
    echo -e "${GREEN}✓${NC} Google Test: Installed"
else
    echo -e "${RED}✗${NC} Google Test: NOT FOUND"
fi

echo ""
echo "📖 Documentation:"
check_command doxygen "" "Doxygen"
check_command dot "" "GraphViz"

echo ""
echo "📊 Coverage Tools:"
check_command lcov "" "lcov"
check_command gcovr "" "gcovr"

echo ""
echo "🐍 Python Tools:"
check_command python3 "" "Python"
check_command pip3 "" "pip3"
check_command pre-commit "" "pre-commit"

echo ""
echo "📦 Package Management:"
check_command vcpkg "" "vcpkg"
if [ -d "$VCPKG_ROOT" ]; then
    echo -e "${GREEN}✓${NC} VCPKG_ROOT: $VCPKG_ROOT"
else
    echo -e "${RED}✗${NC} VCPKG_ROOT not set"
fi

echo ""
echo "🔧 Version Control:"
check_command git "" "Git"
check_command gh "" "GitHub CLI"

echo ""
echo "🚀 Environment Variables:"
echo "  CC=$CC"
echo "  CXX=$CXX"
echo "  VCPKG_ROOT=$VCPKG_ROOT"
echo "  CCACHE_DIR=$CCACHE_DIR"

echo ""
echo "💾 Cache Status:"
if command -v ccache &> /dev/null; then
    echo "ccache statistics:"
    ccache -s | grep -E "(cache size|files in cache|cache hit rate)"
fi

echo ""
echo "🏗️  Test Build:"
echo "Attempting to configure project..."
if [ -f "/workspace/CMakeLists.txt" ]; then
    cd /workspace
    rm -rf build-test
    if cmake -B build-test -G Ninja -DCMAKE_BUILD_TYPE=Debug > /dev/null 2>&1; then
        echo -e "${GREEN}✓${NC} CMake configuration successful"
        rm -rf build-test
    else
        echo -e "${RED}✗${NC} CMake configuration failed"
        echo "  Run manually: cmake -B build -G Ninja"
    fi
else
    echo -e "${YELLOW}⚠${NC}  CMakeLists.txt not found (wrong directory?)"
fi

echo ""
echo "========================================"
echo "✨ Verification Complete!"
echo "========================================"
echo ""
echo "Next steps:"
echo "  1. Configure: cmake -B build -G Ninja"
echo "  2. Build:     cmake --build build"
echo "  3. Test:      ctest --test-dir build"
echo "  4. Run:       ./build/bin/wordNebula"
echo ""

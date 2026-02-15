cmake_minimum_required(VERSION 3.16)

# ============================================================================
# Project Configuration
# ============================================================================
project(
  wordNebula
  VERSION 0.1.0
  DESCRIPTION "A distraction-free terminal-based word processor"
  LANGUAGES CXX
)

# ============================================================================
# Build Options
# ============================================================================
option(BUILD_TESTING "Build tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_SANITIZERS "Enable sanitizers (ASan, UBSan)" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)
option(ENABLE_CPPCHECK "Enable cppcheck analysis" OFF)
option(BUILD_DOCUMENTATION "Build Doxygen documentation" OFF)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "Build type" FORCE
  )
endif()

# ============================================================================
# C++ Standard and Compiler Settings
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# Compiler Warnings
# ============================================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wconversion
    -Wsign-conversion
    -Wformat=2
    -Wimplicit-fallthrough
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/W4 /permissive-)
endif()

# ============================================================================
# Sanitizers (Debug builds)
# ============================================================================
if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE MATCHES "Debug")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
    message(STATUS "Enabled AddressSanitizer and UndefinedBehaviorSanitizer")
  endif()
endif()

# ============================================================================
# Code Coverage
# ============================================================================
if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE MATCHES "Debug")
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
    message(STATUS "Enabled code coverage")
  endif()
endif()

# ============================================================================
# Find Dependencies
# ============================================================================
find_package(Curses REQUIRED)
find_package(spdlog REQUIRED)

# ============================================================================
# Include Directories
# ============================================================================
include_directories(${PROJECT_SOURCE_DIR}/include ${CURSES_INCLUDE_DIR})

# ============================================================================
# Source Files
# ============================================================================
set(WORDNEBULA_SOURCES
    src/Model/TextBuffer.cpp src/Model/GapBuffer.cpp src/Model/WNebulaModel.cpp
    src/View/WNebulaView.cpp src/Presenter/WNebulaPresenter.cpp
    src/WordNebula.cpp
)

# ============================================================================
# Main Executable
# ============================================================================
add_executable(wordNebula ${WORDNEBULA_SOURCES})

target_link_libraries(wordNebula PRIVATE ${CURSES_LIBRARIES} spdlog::spdlog)

target_include_directories(wordNebula PRIVATE ${PROJECT_SOURCE_DIR}/include)

# ============================================================================
# Clang-Tidy Integration
# ============================================================================
if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
  if(CLANG_TIDY_EXE)
    set(CLANG_TIDY_CONF "${CMAKE_SOURCE_DIR}/.clang-tidy")
    set_target_properties(
      wordNebula
      PROPERTIES CXX_CLANG_TIDY
                 "${CLANG_TIDY_EXE};--config-file=${CLANG_TIDY_CONF}"
    )
    message(STATUS "Enabled clang-tidy")
  else()
    message(WARNING "clang-tidy requested but executable not found")
  endif()
endif()

# ============================================================================
# Cppcheck Integration
# ============================================================================
if(ENABLE_CPPCHECK)
  find_program(CPPCHECK_EXE NAMES "cppcheck")
  if(CPPCHECK_EXE)
    set(CPPCHECK_OPTS "--enable=all;--suppress=missingIncludeSystem")
    set_target_properties(
      wordNebula PROPERTIES CXX_CPPCHECK "${CPPCHECK_EXE};${CPPCHECK_OPTS}"
    )
    message(STATUS "Enabled cppcheck")
  else()
    message(WARNING "cppcheck requested but executable not found")
  endif()
endif()

# ============================================================================
# Clang-Format Target
# ============================================================================
file(GLOB_RECURSE ALL_SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp"
     "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)

find_program(CLANG_FORMAT_EXE NAMES "clang-format")
if(CLANG_FORMAT_EXE)
  add_custom_target(
    format
    COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format on source files"
    VERBATIM
  )
endif()

# ============================================================================
# Testing
# ============================================================================
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# ============================================================================
# Documentation
# ============================================================================
if(BUILD_DOCUMENTATION)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    add_subdirectory(docs)
  else()
    message(WARNING "Doxygen not found, documentation will not be built")
  endif()
endif()

# ============================================================================
# Installation
# ============================================================================
install(TARGETS wordNebula RUNTIME DESTINATION bin)

# ============================================================================
# Print Configuration Summary
# ============================================================================
message(STATUS "")
message(STATUS "================================")
message(STATUS "WordNebula Configuration Summary")
message(STATUS "================================")
message(STATUS "Version:           ${PROJECT_VERSION}")
message(STATUS "Build Type:        ${CMAKE_BUILD_TYPE}")
message(
  STATUS
    "C++ Compiler:      ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}"
)
message(STATUS "C++ Standard:      C++${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "Options:")
message(STATUS "  Testing:         ${BUILD_TESTING}")
message(STATUS "  Coverage:        ${ENABLE_COVERAGE}")
message(STATUS "  Sanitizers:      ${ENABLE_SANITIZERS}")
message(STATUS "  clang-tidy:      ${ENABLE_CLANG_TIDY}")
message(STATUS "  cppcheck:        ${ENABLE_CPPCHECK}")
message(STATUS "  Documentation:   ${BUILD_DOCUMENTATION}")
message(STATUS "================================")
message(STATUS "")

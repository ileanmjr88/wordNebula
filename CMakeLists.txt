cmake_minimum_required(VERSION 3.10)

# Set the project name
project(wordNebula VERSION 0.0.1)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Detect the proper system compiler for C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/std:c++20)
    add_compile_options(/std:c11)
else()
    add_compile_options(-std=c++20)
    add_compile_options(-std=c11)
endif()

## Clang Format
# Define the source files to be formatted
file(GLOB_RECURSE ALL_SOURCE_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/Model/*.hpp"
    "${CMAKE_SOURCE_DIR}/lib/*.cpp"
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)

# Add a custom target for clang-format
add_custom_target(
    clang-format
    COMMAND clang-format
    -i
    ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format on source files"
)

# Find the required packages
find_package(Curses REQUIRED)
find_package(spdlog REQUIRED)
include_directories(${CURSES_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/include)

# Add source files
add_executable(wordNebula
    src/Model/TextBuffer.cpp
    src/Model/WNebulaModel.cpp
    src/View/WNebulaView.cpp
    src/Presenter/WNebulaPresenter.cpp
    src/WordNebula.cpp
)
target_link_libraries(wordNebula ${CURSES_LIBRARIES} spdlog::spdlog)
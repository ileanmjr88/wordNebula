cmake_minimum_required(VERSION 3.10)

# Set the project name
project(wordNebula)

# Detect the proper system compiler
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    add_compile_options(/std:c++20)
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()
add_executable(wordNebula WordNebula.cpp) # Uncomment and add your source files here
#!/bin/sh
# This script formats the code using clang-format
# You need to have clang-format installed on your system

for file in $(find ./src ./lib ./include ./tests -name '*.cpp' -o -name '*.hpp'); do
    clang-format --verbose -i "$file"
done
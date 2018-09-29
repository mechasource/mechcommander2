#!/bin/sh
# written for and tested in ubuntu WSL bash

find ./source -type f -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -exec dos2unix -k -q {} \;
find ./source -type f -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -exec clang-format -i -style=file {} \;

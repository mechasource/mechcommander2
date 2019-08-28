#!/bin/sh
# written for and tested in ubuntu WSL bash

find ./source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" -o -name "*.cpp" -o -name "*.c" \) -exec dos2unix -k -q {} \;
unix2dos -k -q source/mc2res/resource.h
find ./source -type f -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -exec clang-format -i -style=file {} \;

# find ./source/mechcmd2 -depth -type f -name "*.hpp" -exec sh -c 'git mv "$1" "${1%.hpp}.h"' _ {} \; 
# for /f "tokens=*" %G in ('@dir /s /b /o:gn "source\*.h"') DO @clang-format.exe -i -style=file "%G"
# for /f "tokens=*" %G in ('@dir /s /b /o:gn "source\*.cpp"') DO @clang-format.exe -i -style=file "%G"
# find ./source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" -o -name "*.cpp" -o -name "*.c" \) -exec sed -i -e 's/\#include \<stuff\/affinematrix.hpp\>/\#include \"stuff\/affinematrix.h\"/g' {} \;
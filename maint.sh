#!/bin/sh
# written for and tested in ubuntu WSL bash
# formatcode="/mnt/d/devapps/compilers/LLVM/bin/clang-format.exe -i -style=file"
# formatcode="clang-format -i -style=file"

find ./source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" -o -name "*.inc" -o -name "*.cpp" -o -name "*.c" -o -name "*.fx" -o -name "*.fxh" -o -name "*.hlsl" -o -name "*.hlsli" \) -exec dos2unix -k -q {} \;
unix2dos -k -q source/mc2res/resource.h
find ./source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" -o -name "*.cpp" -o -name "*.c" \) -exec clang-format -i -style=file {} \;

# find ./source/mechcmd2 -depth -type f -name "*.hpp" -exec sh -c 'git mv "$1" "${1%.hpp}.h"' _ {} \; 
# for /f "tokens=*" %G in ('@dir /s /b /o:gn "source\*.h"') DO @clang-format.exe -i -style=file "%G"
# for /f "tokens=*" %G in ('@dir /s /b /o:gn "source\*.cpp"') DO @clang-format.exe -i -style=file "%G"
# find ./source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.inl" -o -name "*.cpp" -o -name "*.c" \) -exec sed -i -e 's/\#include \<stuff\/affinematrix.hpp\>/\#include \"stuff\/affinematrix.h\"/g' {} \;

# perl -i -pe 's/\bLPERRORLOG\b/IErrorLog\*/g' 

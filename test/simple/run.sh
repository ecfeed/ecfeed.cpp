#!/bin/bash
rm -rf build
mkdir build
cd build
conan install .. --profile ../../../conan_profile --build missing
cmake ..
make
cd bin
./ecfeed
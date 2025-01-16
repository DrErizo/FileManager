#!/bin/bash

cmake . -Bbuild -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=TRUE

cmake --build build -v
cd build
./file-manager
cd ..


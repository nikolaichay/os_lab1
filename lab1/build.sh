#!/bin/bash

if [ -d "build" ]; then
    cd build
else
    mkdir build
    cd build
fi

cmake ..

if [[ $? -eq 0 ]]
then
	make
fi

rm -rf CMakeFiles cmake_install.cmake CMakeCache.txt Makefile compile_commands.json

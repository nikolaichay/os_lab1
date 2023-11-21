#!/bin/bash

cmake .

if [[ $? -eq 0 ]]
then
	make
fi

rm -rf CMakeFiles cmake_install.cmake CMakeCache.txt Makefile compile_commands.json

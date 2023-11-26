cmake_minimum_required (VERSION 3.8)
set(CMAKE_BUILD_TYPE Debug)
project ("daemon")

add_executable (daemon "main.cpp" "daemon.cpp")

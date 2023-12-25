cmake_minimum_required(VERSION 3.14)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

set (
  SRC
  ./host.cpp
  ./client.cpp
  ./connections/connpipe.cpp
  ./connections/connmq.cpp
  ./connections/connshm.cpp
)

add_executable(host ${SRC})

target_include_directories(host PRIVATE  /connections)
target_link_libraries(host PRIVATE pthread rt)



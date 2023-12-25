cmake_minimum_required(VERSION 3.14)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

set (
  SRC
  ./host.cpp
  ./client.cpp
  ./connections/conn_pipe.cpp
  ./connections/conn_mq.cpp
  ./connections/conn_shm.cpp
)

add_executable(host ${SRC})

target_include_directories(host PRIVATE ./src ./src/connections)
target_link_libraries(host PRIVATE pthread rt)



cmake_minimum_required(VERSION 3.10)
project(UDP_OpenCV_Stream)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(server src/server.cpp)
target_link_libraries(server ${OpenCV_LIBS})

add_executable(client src/client.cpp)
target_link_libraries(client ${OpenCV_LIBS})


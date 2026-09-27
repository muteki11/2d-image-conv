cmake_minimum_required(VERSION 3.10)
project(MyTool CXX)

# Set the C++ standard (e.g., C++17 or C++20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the executable target and its source files
add_executable(main src/main.cpp src/imgOp.cpp src/argParse.cpp)

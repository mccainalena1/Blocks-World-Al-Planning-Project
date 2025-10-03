cmake_minimum_required(VERSION 3.10)
project(BlocksWorldAIPlanning)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all .cpp files recursively under src/
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(blocks_world ${SOURCES})

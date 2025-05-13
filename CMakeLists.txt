cmake_minimum_required(VERSION 3.10)

project(CellularAutomaton VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cc)
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)

add_executable(game ${SOURCES} ${HEADERS})

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|MSVC")
    target_compile_options(game PRIVATE -Wall -Wextra -pedantic)
endif()
cmake_minimum_required(VERSION 3.17)
project(DataStructure C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

FILE(GLOB LIB_SRCS ${PROJECT_SOURCE_DIR}/src/*.c)
add_library(DataStructure STATIC ${LIB_SRCS})

add_executable(main main.c)
target_link_libraries(main PRIVATE DataStructure)

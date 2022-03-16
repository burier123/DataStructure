cmake_minimum_required(VERSION 3.17)
project(DataStructure C)

set(CMAKE_C_STANDARD 99)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

FILE(GLOB LIB_SRCS ${PROJECT_SOURCE_DIR}/src/*.c)
add_library(DataStructure STATIC ${LIB_SRCS})
set_target_properties(DataStructure PROPERTIES DEBUG_POSTFIX "D")

#set(CMAKE_DEBUG_POSTFIX "_D")

add_executable(main main.c)
set_target_properties(main PROPERTIES DEBUG_POSTFIX "D")
target_link_directories(main PRIVATE ${LIBRARY_OUTPUT_PATH})
target_link_libraries(main PRIVATE debug DataStructureD optimized DataStructure)


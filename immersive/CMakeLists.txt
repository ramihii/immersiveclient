# vim: et:ts=4:sw=4
cmake_minimum_required(VERSION 3.4)
project(Immersive)

set(Immersive_VERSION_MAJOR 0)
set(Immersive_VERSION_MINOR 1)

option(AS_EXECUTABLE "Whether the output should be an executable" ON)

set(PROJECT_SOURCE_DIR src)
set(PROJECT_BINARY_DIR build/native)

configure_file(
	"${PROJECT_SOURCE_DIR}/immersive_config.h.in"
	"${IMMERSIVE_ROOT}/build/${TGT_ABI}/include/immersive_config.h"
)

set(CMAKE_LIBRARY_PATH "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib")

include_directories("${IMMERSIVE_ROOT}/build/${TGT_ABI}/include")

#include_directories("${PROJECT_BINARY_DIR}")

if(AS_EXECUTABLE)
    add_executable(immersive src/immersive.cpp)
else(AS_EXECUTABLE)
    add_library(immersive SHARED src/immersive.cpp)
endif(AS_EXECUTABLE)

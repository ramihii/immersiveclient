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

#link_directories("~/Android/Sdk/ndk-bundle/platforms/android-27/arch-arm/usr/lib")

#include_directories("${IMMERSIVE_ROOT}/build/${TGT_ABI}/include" "~/Android/Sdk/ndk-bundle/sysroot/usr/include")
include_directories("${IMMERSIVE_ROOT}/build/${TGT_ABI}/include")

#include_directories("${PROJECT_BINARY_DIR}")

if(AS_EXECUTABLE)
    add_executable(immersive src/immersive.cpp)
else(AS_EXECUTABLE)
    add_library(immersive SHARED src/immersive.cpp)
    find_library(log-lib log)
    find_library(android-lib android)
    set(osg-lib "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosg.a")
    set(osg-viewer "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgViewer.a")
    set(osg-db "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgDB.a")
    #set(osg-util "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgUtil.a")
    #set(osg-threads"${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libOpenThreads.a")

    target_link_libraries(immersive ${log-lib} ${osg-lib} ${android-lib} ${osg-viewer} ${osg-db})
    #target_link_libraries(immersive ${log-lib} ${osg-lib} ${android-lib} ${osg-viewer} ${osg-db} ${osg-util} ${osg-threads})
endif(AS_EXECUTABLE)

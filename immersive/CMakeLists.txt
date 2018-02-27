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

set(CMAKE_LIBRARY_PATH "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/")

#include_directories("${IMMERSIVE_ROOT}/build/${TGT_ABI}/include" "~/Android/Sdk/ndk-bundle/sysroot/usr/include")
include_directories("${IMMERSIVE_ROOT}/build/${TGT_ABI}/include/")

#include_directories("${PROJECT_BINARY_DIR}")

#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--whole-archive")

if(AS_EXECUTABLE)
    # TODO: fix
    message("Building this project as executable is not supported currently")
    add_executable(immersive src/immersive.cpp)
else(AS_EXECUTABLE)
    add_library(immersive SHARED src/immersive.cpp)
    add_library(immersive-osg-a-notifier src/osgAndroidNotifier.cpp)

    find_library(log-lib log)
    find_library(android-lib android)

    set(iconv-lib "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libiconv.a")
    set(zbar-lib "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libzbar.a")
    set(osg-lib "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosg.a")
    set(osg-viewer "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgViewer.a")
    set(osg-db "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgDB.a")
    set(osg-util "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgUtil.a")
    set(osg-text "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgText.a")
    set(osg-ga "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libosgGA.a")
    set(openthreads "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/libOpenThreads.a")

    #osg plugins
    set(osg-plugin-obj "${IMMERSIVE_ROOT}/build/${TGT_ABI}/lib/osgPlugins-3.5.10/libosgdb_obj.a")

    #target_link_libraries(immersive ${log-lib} ${osg-lib} ${iconv-lib} ${zbar-lib})
    #target_link_libraries(immersive ${log-lib} ${osg-lib} ${android-lib} ${osg-viewer} ${osg-db} ${osg-util} ${osg-threads})
    target_link_libraries(immersive immersive-osg-a-notifier ${log-lib} ${android-lib} "-Wl,--whole-archive" ${openthreads} ${osg-util} ${osg-text} ${osg-lib} ${osg-ga} ${osg-viewer} ${osg-db} ${iconv-lib} ${zbar-lib} ${osg-plugin-obj} "-Wl,--no-whole-archive")
endif(AS_EXECUTABLE)

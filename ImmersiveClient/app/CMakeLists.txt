# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

project(aruco_java LANGUAGES CXX)

# TODO move this to Gradle (should actually be in a machine specific config
set(OpenCV_DIR C:/01-libs/OpenCV-android-sdk/sdk/native/jni)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS ON) #...with compiler extensions like gnu++11

# Check that the OpenCV Java library exists
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libopencv_java3.so)
    message(FATAL "OpenCV Java lib doesn't exist for ARCH=${ANDROID_ABI}")
endif()

add_library( aruco_java SHARED
    src/main/cpp/aruco_java.cpp
    src/main/cpp/aruco_java.h
        )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/main/include)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

# ARuco c++ lib
add_library( aruco
             SHARED
             IMPORTED )
set_target_properties(
        aruco

       # Specifies the parameter you want to define.
       PROPERTIES IMPORTED_LOCATION

       # Provides the path to the library you want to import.
       ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libaruco.so
       )

set( ARUCO_LIB aruco )

find_library( log-lib
              log )

target_link_libraries( # Specifies the target library.
                       aruco_java
                       ${ARUCO_LIB}

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

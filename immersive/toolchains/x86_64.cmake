set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_SYSROOT $ENV{TOOLCHAIN_HOME}/$ENV{TOOLDIR_x86_64}/sysroot)

set(CMAKE_C_COMPILER x86_64-linux-android-gcc)
set(CMAKE_CXX_COMPILER x86_64-linux-android-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
